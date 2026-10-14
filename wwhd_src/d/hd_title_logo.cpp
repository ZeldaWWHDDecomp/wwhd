/* HD title logo parts; private binary-derived source. */
#include "d/hd_title_parts.h"
#include "d/hd_parts_lyt.h"

static const u32 titleLogoStringDestructor = 0x026EBD34;
extern void titleLogoDestroyStringArray(void*, u32, u32, u32, u32, u32);
struct LogoTextPaneNamesArray_l { hdUiSafeString mNames[8]; };
static LogoTextPaneNamesArray_l sLogoTextPaneNames;
struct LogoSpecialPaneNamesArray_l { hdUiSafeString mNames[3]; };
static LogoSpecialPaneNamesArray_l sLogoSpecialPaneNames;
struct LogoPaneNamesArray_l { hdUiSafeString mNames[8]; };
static LogoPaneNamesArray_l sLogoPaneNames;

static hdPartsVtbl_c sTitleLogoVtable;
static u8 sTitleLogoHolderVtable;
static u8 sTitleLogoSecondaryVtable;
static hdPartsStateId_c sTitleLogoIntroState;
static hdPartsStateId_c sTitleLogoLoopState;
extern void* titleLogoAllocate(u32);
extern void titleLogoDelete(void*);
extern void titleConstructLogoBase(hdParts_c*);
extern void titleDestroyLogoBase(hdParts_c*, u32);
extern void titleLogoStartAnimation(hdAnimSet*, u32, u32, f32);
extern u32 titleLogoAnimationEnded(hdAnimSet*, u32);
extern void titleSetAnimationPlaying(hdAnimSet*, u32, u32);

struct hdTitleLogoPaneNamesGuard_l { be<u32> mInitialized; };
static hdTitleLogoPaneNamesGuard_l sLogoPaneNamesGuard;
static hdUiStringTable sLogoPaneNameStringTable;
static u8 sLogoPaneNamesRegistration;
extern void titleLogoRegisterPaneNames(void*);

/* 026EB4E8 */
void hdTitleLogo_c::findLogoPanes() {
    if (sLogoPaneNamesGuard.mInitialized == 0) {
        sLogoPaneNamesGuard.mInitialized = 1;
        sLogoPaneNames.mNames[0].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[0].mText.v = gabi::guest_addr("P_ZeldaRuby_00");
        sLogoPaneNames.mNames[1].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[1].mText.v = gabi::guest_addr("P_TitleLogoZelda_00");
        sLogoPaneNames.mNames[2].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[2].mText.v = gabi::guest_addr("P_WindwakerJ_00");
        sLogoPaneNames.mNames[3].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[3].mText.v = gabi::guest_addr("P_TitleLogoHDJ_00");
        sLogoPaneNames.mNames[4].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[4].mText.v = gabi::guest_addr("P_TitleLogoHD_00");
        sLogoPaneNames.mNames[5].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[5].mText.v = gabi::guest_addr("P_Windwaker_00");
        sLogoPaneNames.mNames[6].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[6].mText.v = gabi::guest_addr("P_WindwakerJMask_00");
        sLogoPaneNames.mNames[7].mVtable = &sLogoPaneNameStringTable;
        sLogoPaneNames.mNames[7].mText.v = gabi::guest_addr("P_WindwakerMask_00");
        titleLogoRegisterPaneNames(&sLogoPaneNamesRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 8; ++index) {
        hdUiSafeString* name = &sLogoPaneNames.mNames[index];
        hdScreenResourceTable* table = root->mVtable;
        // The binary forwards the destination-array byte offset in r4 to assurance.
        gabi::call_ptr(name->mVtable->mAssureTermination.get(), name, index * sizeof(gptr<void>));
        mLogoPanes[index] = gabi::call_ptr<void*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
}

static hdTitleLogoPaneNamesGuard_l sLogoTextPaneNamesGuard;
static u8 sLogoTextPaneNamesRegistration;

/* 026EAFE8 */
void hdTitleLogo_c::findTextPanes() {
    if (sLogoTextPaneNamesGuard.mInitialized == 0) {
        sLogoTextPaneNamesGuard.mInitialized = 1;
        sLogoTextPaneNames.mNames[0].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_00");
        sLogoTextPaneNames.mNames[1].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[1].mText.v = gabi::guest_addr("N_TitleLogo_00_JpJa");
        sLogoTextPaneNames.mNames[2].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[2].mText.v = gabi::guest_addr("N_TitleLogo_00");
        sLogoTextPaneNames.mNames[3].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[3].mText.v = gabi::guest_addr("N_WindwakerJ_00");
        sLogoTextPaneNames.mNames[4].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[4].mText.v = gabi::guest_addr("N_Windwaker_00");
        sLogoTextPaneNames.mNames[5].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[5].mText.v = gabi::guest_addr("N_Text_00");
        sLogoTextPaneNames.mNames[6].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[6].mText.v = gabi::guest_addr("N_Logo_00");
        sLogoTextPaneNames.mNames[7].mVtable = &sLogoPaneNameStringTable;
        sLogoTextPaneNames.mNames[7].mText.v = gabi::guest_addr("N_Text_01");
        titleLogoRegisterPaneNames(&sLogoTextPaneNamesRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 8; ++index) {
        hdUiSafeString* name = &sLogoTextPaneNames.mNames[index];
        hdScreenResourceTable* table = root->mVtable;
        // The indirect assurance call also carries the destination-array byte offset in r4.
        // Preserve it when a generated input resolves a different virtual target.
        gabi::call_ptr(name->mVtable->mAssureTermination.get(), name, index * sizeof(gptr<void>));
        mTextPanes[index] = gabi::call_ptr<void*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
}

static hdTitleLogoPaneNamesGuard_l sLogoSpecialNamesGuard;
static u8 sLogoSpecialNamesRegistration;
static hdTitleLogoPaneNamesGuard_l sLogoTextTypeGuard;
static hdTitleLogoPaneNamesGuard_l sLogoPaneTypeGuard;
static hdTitleLogoPaneNamesGuard_l sLogoLayoutTypeGuard;
static hdTitleLogoPaneNamesGuard_l sLogoRootTypeGuard;
static hdScreenType sLogoTextType;
static hdScreenType sLogoPaneType;
static hdScreenType sLogoLayoutType;
static hdScreenType sLogoRootType;

/* 026EB250 */
void hdTitleLogo_c::findSpecialPanes() {
    if (sLogoSpecialNamesGuard.mInitialized == 0) {
        sLogoSpecialNamesGuard.mInitialized = 1;
        sLogoSpecialPaneNames.mNames[0].mVtable = &sLogoPaneNameStringTable;
        sLogoSpecialPaneNames.mNames[0].mText.v = gabi::guest_addr("T_Press_00");
        sLogoSpecialPaneNames.mNames[1].mVtable = &sLogoPaneNameStringTable;
        sLogoSpecialPaneNames.mNames[1].mText.v = gabi::guest_addr("T_Nintendo_00");
        sLogoSpecialPaneNames.mNames[2].mVtable = &sLogoPaneNameStringTable;
        sLogoSpecialPaneNames.mNames[2].mText.v = gabi::guest_addr("T_Version_00");
        titleLogoRegisterPaneNames(&sLogoSpecialNamesRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 3; ++index) {
        hdUiSafeString* name = &sLogoSpecialPaneNames.mNames[index];
        hdScreenResourceTable* table = root->mVtable;
        name->assureTermination();
        auto* pane = gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        if (sLogoTextTypeGuard.mInitialized == 0) {
            u32 paneReady = sLogoPaneTypeGuard.mInitialized;
            sLogoTextTypeGuard.mInitialized = 1;
            if (paneReady == 0) {
                u32 layoutReady = sLogoLayoutTypeGuard.mInitialized;
                sLogoPaneTypeGuard.mInitialized = 1;
                if (layoutReady == 0) {
                    u32 rootReady = sLogoRootTypeGuard.mInitialized;
                    sLogoLayoutTypeGuard.mInitialized = 1;
                    if (rootReady == 0) {
                        sLogoRootType.mParent = NULL;
                        sLogoRootTypeGuard.mInitialized = 1;
                    }
                    sLogoLayoutType.mParent = &sLogoRootType;
                }
                sLogoPaneType.mParent = &sLogoLayoutType;
            }
            sLogoTextType.mParent = &sLogoPaneType;
        }
        bool matches = false;
        if (pane != NULL) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type != NULL) {
                if (type == &sLogoTextType) { matches = true; break; }
                type = type->mParent;
            }
        }
        mSpecialPanes[index] = matches ? pane : NULL;
    }
}

/* 026EAC78 */
void hdTitleLogo_c::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sTitleLogoHolderVtable;
    mSecondaryVtable = &sTitleLogoSecondaryVtable;
    titleDestroyLogoBase(this, 0);
    if ((flags & 1) != 0) titleLogoDelete(this);
}

/* 026EACE4 */
void hdTitleLogo_c::clearAdvanceRequest() {
    mAdvanceRequested = 0;
}

/* 026EACF0 */
void hdTitleLogo_c::updateIdle() {
    vCalcLayout();
    if (mSkipRequested != 0) {
        mShortIntro = 1;
        changeState(&sTitleLogoIntroState);
    } else if (mAdvanceRequested != 0 || mShortIntro != 0) {
        changeState(&sTitleLogoIntroState);
    }
}

/* 026EAD78 */
void hdTitleLogo_c::beginIntro() {
    u8 shortIntro = mShortIntro;
    hdPartsLayout_c* layout = mpLayout;
    mIntroStarted = 1;
    hdAnimSet* animation = layout->animSet();
    if (shortIntro != 0) {
        titleSetAnimationPlaying(animation, 1, 1);
        vCalcLayout();
    } else {
        titleLogoStartAnimation(animation, 0, 1, 1.0f);
    }
}

/* 026EAE04 */
void hdTitleLogo_c::updateIntro() {
    vCalcLayout();
    u8 shortIntro = mShortIntro;
    hdPartsLayout_c* layout = mpLayout;
    if (shortIntro != 0) {
        if (titleLogoAnimationEnded(layout->animSet(), 1) != 0) changeState(&sTitleLogoLoopState);
    } else {
        u8 skip = mSkipRequested;
        hdAnimSet* animation = layout->animSet();
        if (skip != 0) {
            titleSetAnimationPlaying(animation, 0, 1);
            changeState(&sTitleLogoLoopState);
        } else if (titleLogoAnimationEnded(animation, 0) != 0) {
            changeState(&sTitleLogoLoopState);
        }
    }
}

/* 026EAEB0 */
void hdTitleLogo_c::beginLoop() {
    titleLogoStartAnimation(animSet(), 2, 0, 1.0f);
    titleLogoStartAnimation(animSet(), 3, 2, 1.0f);
}

/* 026EAF28 */
void hdTitleLogo_c::calculateLayout() {
    vCalcLayout();
}

/* 026EAF38 */
hdTitleLogo_c* hdTitleLogo_c::construct(hdTitleLogo_c* self) {
    if (self == NULL) {
        self = static_cast<hdTitleLogo_c*>(titleLogoAllocate(sizeof(hdTitleLogo_c)));
        if (self == NULL) return NULL;
    }
    titleConstructLogoBase(self);
    self->mVtable = &sTitleLogoVtable;
    self->mSecondaryVtable = &sTitleLogoSecondaryVtable;
    self->mIntroStarted = 0;
    self->mShortIntro = 0;
    self->mStateHolder.mHolderVtable = &sTitleLogoHolderVtable;
    self->mAdvanceRequested = 0;
    self->mSkipRequested = 0;
    if (gabi::ea(self->mTextPanes) == 0) titleLogoAllocate(sizeof(self->mTextPanes));
    if (gabi::ea(self->mLogoPanes) == 0) titleLogoAllocate(sizeof(self->mLogoPanes));
    if (gabi::ea(self->mSpecialPanes) == 0) titleLogoAllocate(sizeof(self->mSpecialPanes));
    return self;
}


/* 026EAC0C */
static void titleLogoDestroyLogoTextPaneNames() {
    titleLogoDestroyStringArray(sLogoTextPaneNames.mNames, 8, sizeof(hdUiSafeString), titleLogoStringDestructor, 0, 0);
}

/* 026EAC30 */
static void titleLogoDestroyLogoSpecialPaneNames() {
    titleLogoDestroyStringArray(sLogoSpecialPaneNames.mNames, 3, sizeof(hdUiSafeString), titleLogoStringDestructor, 0, 0);
}

/* 026EAC54 */
static void titleLogoDestroyLogoPaneNames() {
    titleLogoDestroyStringArray(sLogoPaneNames.mNames, 8, sizeof(hdUiSafeString), titleLogoStringDestructor, 0, 0);
}

/* WWHD glue (tools/verify/autoglue.py): title logo parts
 data sTitleLogoVtable 10108320
 data sTitleLogoHolderVtable 10108430
 data sTitleLogoSecondaryVtable 10108440
 data sTitleLogoIntroState 1049CFA4
 data sTitleLogoLoopState 1049CFD4
 ext 0273AD10 void* titleLogoAllocate(u32)
 ext 0273AF40 void titleLogoDelete(void*)
 ext 026F89E8 void titleConstructLogoBase(hdParts_c*)
 ext 026F88F0 void titleDestroyLogoBase(hdParts_c*,u32)
 ext 020053E4 void titleLogoStartAnimation(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 titleLogoAnimationEnded(hdAnimSet*,u32)
 ext 02005708 void titleSetAnimationPlaying(hdAnimSet*,u32,u32)
 ext 028F0164 void titleLogoDestroyStringArray(void*,u32,u32,u32,u32,u32)
 data sLogoTextPaneNames 1049CE88 nocheck
 data sLogoSpecialPaneNames 1049CEC8 nocheck
 data sLogoPaneNames 1049CEE0 nocheck
 data sLogoPaneNamesGuard 1049CF70
 data sLogoPaneNameStringTable 10108074
 data sLogoPaneNamesRegistration 101F7000
 ext 028F026C void titleLogoRegisterPaneNames(void*)
 str 10108220
 str 101081E4
 str 101081D4
 str 1010820C
 str 10108230
 str 10108244
 str 101081F8
 str 10108254
 data sLogoTextPaneNamesGuard 1049CF68
 data sLogoTextPaneNamesRegistration 101F6FE8
 str 1010817C in 026EAFE8
 str 10108144
 str 10108188
 str 10108134
 str 10108198
 str 10108158
 str 10108164
 str 10108170
 data sLogoSpecialNamesGuard 1049CF6C
 data sLogoSpecialNamesRegistration 101F6FF4
 data sLogoTextTypeGuard 101FDD40
 data sLogoPaneTypeGuard 101FD8F0
 data sLogoLayoutTypeGuard 101FD89C
 data sLogoRootTypeGuard 101FD888
 data sLogoTextType 101FDD3C
 data sLogoPaneType 101FD8F4
 data sLogoLayoutType 101FD8F8
 data sLogoRootType 101FD8FC
 str 101081B8
 str 101081A8
 str 101081C4
*/

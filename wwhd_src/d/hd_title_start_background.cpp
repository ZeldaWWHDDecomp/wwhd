/* HD title start-sequence background; private binary-derived source. */
#include "d/hd_title_parts.h"

static const u32 titleBackgroundStringDestructor = 0x026E5B54;
extern void titleBackgroundDestroyStringArray(void*, u32, u32, u32, u32, u32);
struct BackgroundLayoutNameArray_l { hdUiSafeString mNames[1]; };
static BackgroundLayoutNameArray_l sBackgroundLayoutName;

static hdPartsStateId_c sBackgroundAnimationEndState;
static hdPartsStateId_c sBackgroundIdleState;
static hdPartsVtbl_c sBackgroundPartsVtable;
static u8 sBackgroundHolderVtable;
static u8 sBackgroundSecondaryVtable;
static hdUiStringTable sBackgroundStringVtable;
static hdUiSafeString sBackgroundFirstCaption;
extern void* titleAllocate(u32);
extern void titleDelete(void*);
extern void titleConstructBackgroundBase(hdParts_c*);
extern void titleDestroyBackgroundBase(hdParts_c*, u32);
extern void titleStartAnimation(hdAnimSet*, u32, u32, f32);
extern u32 titleAnimationEnded(hdAnimSet*, u32);
extern void titleSetHeaderText(void*, hdUiSafeString*, hdUiSafeString*);
extern f32 titleHeaderTextWidth(void*);

static hdUiSafeString sBackgroundArchiveName;
static hdUiSafeString sBackgroundScreenLayoutName;
static hdPartsStateId_c sBackgroundInitialState;
struct hdTitleBackgroundNameGuard_l { be<u32> mInitialized; };
static hdTitleBackgroundNameGuard_l sBackgroundWindowNameGuard;
static u8 sBackgroundWindowNameRegistration;
extern void titleBackgroundAttachArchives(hdParts_c*, hdUiSafeString*, u32);
extern void titleBackgroundAssureName(hdUiSafeString*);
extern void titleBackgroundRegisterName(void*);

/* 026E52AC */
u32 hdTitleStartBackground_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    titleBackgroundAttachArchives(this, &sBackgroundArchiveName, 1);
    if (vLoad(&sBackgroundScreenLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc) == 0)
        return 0;
    {
        gabi::Local<hdUiSafeString> name;
        name->mVtable = &sBackgroundStringVtable;
        name->mText.v = gabi::guest_addr("T_Header_00");
        auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
        hdScreenResourceTable* table = root->mVtable;
        titleBackgroundAssureName(name.get());
        mpHeaderText = gabi::call_ptr<void*>(table->mFindPane.get(), root, name->mText.get(), 1);
        titleBackgroundDestroyStringArray(name.get(), 1, sizeof(hdUiSafeString),
            titleBackgroundStringDestructor, 0, 0);
    }
    hdUiSafeString* windowName = &sBackgroundLayoutName.mNames[0];
    if (sBackgroundWindowNameGuard.mInitialized == 0) {
        windowName->mVtable = &sBackgroundStringVtable;
        sBackgroundWindowNameGuard.mInitialized = 1;
        windowName->mText.v = gabi::guest_addr("W_Header_00");
        titleBackgroundRegisterName(&sBackgroundWindowNameRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    hdScreenResourceTable* table = root->mVtable;
    windowName->assureTermination();
    auto* window = gabi::call_ptr<hdTitleBackgroundPane_l*>(table->mFindPane.get(), root,
        windowName->mText.get(), 1);
    hdPartsLayout_c* layout = mpLayout;
    mpHeaderWindow = window;
    titleStartAnimation(layout->animSet(), 1, 1, 1.0f);
    auto* data = reinterpret_cast<hdTitleBackgroundLayoutData_l*>(mpLayout->lyt());
    hdUiSafeString resources;
    resources.mVtable = &sBackgroundStringVtable;
    resources.mText = data->mpTextResources;
    titleSetHeaderText(mpHeaderText, &resources, &sBackgroundFirstCaption);
    void* text = mpHeaderText;
    hdTitleBackgroundPane_l* oldWindow = mpHeaderWindow;
    f32 width = titleHeaderTextWidth(text);
    hdTitleBackgroundPane_l* currentWindow = mpHeaderWindow;
    u8 flags = oldWindow->mFlags;
    f32 y = currentWindow->mY;
    oldWindow->mX = gabi::fadds_ppc(width, 250.0f);
    oldWindow->mFlags = flags | 0x10;
    oldWindow->mY = y;
    startState(&sBackgroundInitialState);
    return 1;
}

/* 026E4E9C */
void hdTitleStartBackground_c::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sBackgroundHolderVtable;
    mSecondaryVtable = &sBackgroundSecondaryVtable;
    titleDestroyBackgroundBase(this, 0);
    if ((flags & 1) != 0) titleDelete(this);
}

/* 026E4F08 */
void hdTitleStartBackground_c::beginBackgroundAnimation() {
    titleStartAnimation(animSet(), 0, 0, 1.0f);
    gabi::call_ptr(mVtable->mExecuteChildren.get(), this);
    vCalcLayout();
}

/* 026E4F74 */
void hdTitleStartBackground_c::waitBackgroundAnimation() {
    if (titleAnimationEnded(animSet(), 0) != 0) {
        changeState(&sBackgroundIdleState);
    }
    vCalcLayout();
}

/* 026E4FD8 */
void hdTitleStartBackground_c::beginCaptionAnimation() {
    titleStartAnimation(animSet(), 3, 2, 1.0f);
    mAnimationPhase = 0;
}

/* 026E5024 */
void hdTitleStartBackground_c::updateCaptionAnimation() {
    switch (u32(mAnimationPhase)) {
    case 0:
        if (titleAnimationEnded(animSet(), 3) != 0) {
            auto* data = reinterpret_cast<hdTitleBackgroundLayoutData_l*>(mpLayout->lyt());
            be<u8>* textResources = data->mpTextResources;
            u32 caption = mCaptionIndex;
            hdUiSafeString resources;
            resources.mVtable = &sBackgroundStringVtable;
            resources.mText = textResources;
            hdUiSafeString* name = hdPartsView<hdUiSafeString>(
                gabi::ea(&sBackgroundFirstCaption) + caption * sizeof(hdUiSafeString));
            titleSetHeaderText(mpHeaderText, &resources, name);
            void* text = mpHeaderText;
            hdTitleBackgroundPane_l* window = mpHeaderWindow;
            f32 width = titleHeaderTextWidth(text);
            hdTitleBackgroundPane_l* currentWindow = mpHeaderWindow;
            u8 flags = window->mFlags;
            f32 y = currentWindow->mY;
            f32 x = gabi::fadds_ppc(width, 250.0f);
            window->mFlags = flags | 0x10;
            window->mY = y;
            window->mX = x;
            titleStartAnimation(animSet(), 2, 2, 1.0f);
            u32 phase = mAnimationPhase;
            hdPartsVtbl_c* table = mVtable;
            mAnimationPhase = phase + 1;
            gabi::call_ptr(table->mCalcLayout.get(), this);
            return;
        }
        break;
    case 1:
        if (titleAnimationEnded(animSet(), 2) != 0) {
            changeState(&sBackgroundIdleState);
        }
        break;
    default:
        break;
    }
    vCalcLayout();
}

/* 026E5184 */
void hdTitleStartBackground_c::calculateLayout() {
    vCalcLayout();
}

/* 026E5194 */
void hdTitleStartBackground_c::startHeaderAnimation() {
    titleStartAnimation(animSet(), HEADER_ANIMATION, 0, 1.0f);
}

/* 026E51B0 */
void hdTitleStartBackground_c::waitHeaderAnimation() {
    if (titleAnimationEnded(animSet(), HEADER_ANIMATION) != 0) {
        changeState(&sBackgroundAnimationEndState);
    }
    vCalcLayout();
}

/* 026E5214 */
hdTitleStartBackground_c* hdTitleStartBackground_c::construct(hdTitleStartBackground_c* self) {
    if (self == NULL) {
        self = static_cast<hdTitleStartBackground_c*>(titleAllocate(sizeof(hdTitleStartBackground_c)));
        if (self == NULL) return NULL;
    }
    titleConstructBackgroundBase(self);
    self->mVtable = &sBackgroundPartsVtable;
    self->mSecondaryVtable = &sBackgroundSecondaryVtable;
    self->mAnimationPhase = 0;
    self->mStateHolder.mHolderVtable = &sBackgroundHolderVtable;
    self->mCaptionIndex = 0;
    if (gabi::ea(&self->mpHeaderText) == 0) titleAllocate(sizeof(gptr<void>));
    if (gabi::ea(&self->mpHeaderWindow) == 0) titleAllocate(sizeof(gptr<void>));
    return self;
}


/* 026E4E78 */
static void titleBackgroundDestroyBackgroundLayoutName() {
    titleBackgroundDestroyStringArray(sBackgroundLayoutName.mNames, 1, sizeof(hdUiSafeString), titleBackgroundStringDestructor, 0, 0);
}

/* WWHD glue (tools/verify/autoglue.py): title start-sequence background
 data sBackgroundAnimationEndState 1049C5A8
 data sBackgroundIdleState 1049C540
 data sBackgroundPartsVtable 10106CB8
 data sBackgroundHolderVtable 10106DE0
 data sBackgroundSecondaryVtable 10106DF0
 data sBackgroundStringVtable 101069B0
 data sBackgroundFirstCaption 1049C474
 ext 0273AD10 void* titleAllocate(u32)
 ext 0273AF40 void titleDelete(void*)
 ext 026F89E8 void titleConstructBackgroundBase(hdParts_c*)
 ext 026F88F0 void titleDestroyBackgroundBase(hdParts_c*,u32)
 ext 027045B0 void titleSetHeaderText(void*,hdUiSafeString*,hdUiSafeString*)
 ext 02704A08 f32 titleHeaderTextWidth(void*)
 ext 020053E4 void titleStartAnimation(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 titleAnimationEnded(hdAnimSet*,u32)
 ext 028F0164 void titleBackgroundDestroyStringArray(void*,u32,u32,u32,u32,u32)
 data sBackgroundLayoutName 1049C408 nocheck
 data sBackgroundArchiveName 1049C410
 data sBackgroundScreenLayoutName 1049C5A0
 data sBackgroundInitialState 1049C4E0
 data sBackgroundWindowNameGuard 1049C4DC
 data sBackgroundWindowNameRegistration 101F6EC8
 str 10106AC0
 str 10106ACC
 ext 026F90D8 void titleBackgroundAttachArchives(hdParts_c*,hdUiSafeString*,u32)
 ext 026E5B68 void titleBackgroundAssureName(hdUiSafeString*)
 ext 028F026C void titleBackgroundRegisterName(void*)
*/

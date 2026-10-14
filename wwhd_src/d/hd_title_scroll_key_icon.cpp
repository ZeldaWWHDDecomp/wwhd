/* HD file-selection ScrollKeyIcon_00 companion; merged file-select code stays separate. */
#include "d/hd_title_parts.h"

static hdPartsVtbl_c sScrollKeyPartsVtable;
static u8 sScrollKeyHolderVtable;
static u8 sScrollKeySecondaryVtable;
static hdUiSafeString sScrollKeyResourceName;
struct hdTitleScrollKeyGlobals_l {
    be<u32> mHead;
    be<u32> mTail;
    be<u32> mOwner;
    be<u32> mCount;
};
WWHD_SIZE(hdTitleScrollKeyGlobals_l, 16);
static hdTitleScrollKeyGlobals_l sScrollKeyGlobals;
struct hdTitleScrollKeyAngle_l { be<f32> mValue; };
WWHD_SIZE(hdTitleScrollKeyAngle_l, 4);
static hdTitleScrollKeyAngle_l sScrollKeyMinimumAngle;
static hdTitleScrollKeyAngle_l sScrollKeyMaximumAngle;
static hdTitleScrollKeyAngle_l sScrollKeyMinimumAngleValue;
static hdTitleScrollKeyAngle_l sScrollKeyMaximumAngleValue;
static u8 sScrollKeyPrimaryFlag;
static u8 sScrollKeySecondaryFlag;
static u8 sScrollKeyGlobalsRegistration;
static u8 sScrollKeyPrimaryRegistration;
static u8 sScrollKeySecondaryRegistration;
static hdUiStringTable sScrollKeyStringVtable;
extern void* titleScrollKeyAllocate(u32);
extern void titleScrollKeyDelete(void*);
extern void titleScrollKeyConstructBase(hdPartsKeyIcon_c*);
extern void titleScrollKeyDestroyBase(hdPartsKeyIcon_c*, u32);
extern u32 titleScrollKeyInitializeBase(hdPartsKeyIcon_c*, hdUiSafeString*, u32, u32, hdPartsDesc_c*);
extern void titleScrollKeyRegisterGlobal(void*);
extern void titleScrollKeyConstructPrimaryFlag(void*);
extern void titleScrollKeyConstructSecondaryFlag(void*);

/* 026D3944 */
void hdTitleScrollKeyIcon_c::destroy(hdTitleScrollKeyIcon_c* self, u32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sScrollKeyHolderVtable;
    self->mSecondaryVtable = &sScrollKeySecondaryVtable;
    titleScrollKeyDestroyBase(self, 0);
    if ((flags & 1) != 0) titleScrollKeyDelete(self);
}

/* 026D39B0 */
hdTitleScrollKeyIcon_c* hdTitleScrollKeyIcon_c::construct(hdTitleScrollKeyIcon_c* self) {
    if (self == NULL) {
        self = static_cast<hdTitleScrollKeyIcon_c*>(titleScrollKeyAllocate(sizeof(hdTitleScrollKeyIcon_c)));
        if (self == NULL) return NULL;
    }
    titleScrollKeyConstructBase(self);
    self->mVtable = &sScrollKeyPartsVtable;
    self->mStateHolder.mHolderVtable = &sScrollKeyHolderVtable;
    self->mSecondaryVtable = &sScrollKeySecondaryVtable;
    return self;
}

/* 026D3A1C */
u32 hdTitleScrollKeyIcon_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    return titleScrollKeyInitializeBase(this, &sScrollKeyResourceName, arg1, arg2, desc);
}

/* 026D3A34 */
static void initializeTitleScrollKeyGlobals() {
    sScrollKeyGlobals.mCount = 0;
    sScrollKeyGlobals.mOwner = 0;
    sScrollKeyGlobals.mTail = 0;
    sScrollKeyGlobals.mHead = 0;
    titleScrollKeyRegisterGlobal(&sScrollKeyGlobalsRegistration);
    f32 minimum = sScrollKeyMinimumAngleValue.mValue;
    f32 maximum = sScrollKeyMaximumAngleValue.mValue;
    sScrollKeyMinimumAngle.mValue = minimum;
    sScrollKeyMaximumAngle.mValue = maximum;
    titleScrollKeyConstructPrimaryFlag(&sScrollKeyPrimaryFlag);
    titleScrollKeyRegisterGlobal(&sScrollKeyPrimaryRegistration);
    titleScrollKeyConstructSecondaryFlag(&sScrollKeySecondaryFlag);
    titleScrollKeyRegisterGlobal(&sScrollKeySecondaryRegistration);
    sScrollKeyResourceName.mVtable = &sScrollKeyStringVtable;
    sScrollKeyResourceName.mText.v = gabi::guest_addr("ScrollKeyIcon_00");
}

/* WWHD glue (tools/verify/autoglue.py): ScrollKeyIcon_00 companion
 data sScrollKeyPartsVtable 10102818
 data sScrollKeyHolderVtable 10102940
 data sScrollKeySecondaryVtable 10102950
 data sScrollKeyResourceName 1049A8EC
 data sScrollKeyGlobals 1049A8DC
 data sScrollKeyMinimumAngle 1049A8D0
 data sScrollKeyMaximumAngle 1049A8D4
 data sScrollKeyMinimumAngleValue 101027FC
 data sScrollKeyMaximumAngleValue 10102800
 data sScrollKeyPrimaryFlag 1049A8D8
 data sScrollKeySecondaryFlag 1049A8D9
 data sScrollKeyGlobalsRegistration 101F69EC
 data sScrollKeyPrimaryRegistration 101F69F8
 data sScrollKeySecondaryRegistration 101F6A04
 data sScrollKeyStringVtable 101027DC
 str 10102804
 ext 0273AD10 void* titleScrollKeyAllocate(u32)
 ext 0273AF40 void titleScrollKeyDelete(void*)
 ext 026F80FC void titleScrollKeyConstructBase(hdPartsKeyIcon_c*)
 ext 026F7DAC void titleScrollKeyDestroyBase(hdPartsKeyIcon_c*,u32)
 ext 026F8174 u32 titleScrollKeyInitializeBase(hdPartsKeyIcon_c*,hdUiSafeString*,u32,u32,hdPartsDesc_c*)
 ext 028F026C void titleScrollKeyRegisterGlobal(void*)
 ext 028ED6F8 void titleScrollKeyConstructPrimaryFlag(void*)
 ext 028EAB2C void titleScrollKeyConstructSecondaryFlag(void*)
*/

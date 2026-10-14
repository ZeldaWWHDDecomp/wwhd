/* HD BatonInputPoint_01: binds the completion animation to its layout. */
#include "d/hd_screen_baton_completion_point.h"
#include "d/hd_layout_mgr.h"

static hdPartsVtbl_c sBatonInputScreenVtable;
static u8 sBatonInputHolderVtable;
static u8 sBatonInputSecondaryVtable;
static hdUiStringTable sBatonInputStringVtable;
struct hdBatonInputHeaderWords_c { be<u32> mWords[4]; };
struct hdBatonInputRegistration_c { u8 mStorage[12]; };
struct hdBatonInputStrings_c { hdUiSafeString mEntries[1]; };
static hdBatonInputHeaderWords_c sBatonInputHeaderWords;
static hdBatonInputRegistration_c sBatonInputHeaderRegistration;
static hdBatonInputRegistration_c sBatonInputRotationRegistration;
static hdBatonInputRegistration_c sBatonInputPositiveRotationRegistration;
static f32 sBatonInputMinimumRotation;
static f32 sBatonInputMaximumRotation;
static u8 sBatonInputRotationGuard;
static u8 sBatonInputPositiveRotationGuard;
static hdBatonInputStrings_c sBatonInputAnimationGroups;
static hdBatonInputStrings_c sBatonInputAnimationNames;
static hdUiSafeString sBatonInputScreenName;
static u32 sBatonInputAnimationGroupIndex;
using hdBatonUiSystemPointer = gptr<hdScreenUiSystem>;
static hdBatonUiSystemPointer sBatonInputUiSystem;
static void destroyBatonInputMessageBase(OneTimePart_l* self, s32 flags);
static void constructBatonInputMessageBase(OneTimePart_l* self);
static hdBatonCompletionPoint_c* allocateBatonInputPoint(u32 size);
static void deleteBatonInputPoint(void* object);
static u32 loadBatonInputMessageBase(OneTimePart_l* self, hdUiSafeString* name, u32 first, u32 second, void* descriptor);
static hdPartsLayout_c* allocateBatonInputLayout(u32 size, hdScreenHeap* heap, u32 alignment);
static void bindBatonInputAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group);
static void registerBatonInputGlobal(hdBatonInputRegistration_c* registration);
static void initializeBatonRotation(be<u8>* guard);
static void initializePositiveBatonRotation(be<u8>* guard);

/* 0262048C */
void hdBatonCompletionPoint_c::destroy(hdBatonCompletionPoint_c* self, s32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sBatonInputHolderVtable;
    self->mSecondaryVtable = &sBatonInputSecondaryVtable;
    destroyBatonInputMessageBase(self, 0);
    if (flags & 1) deleteBatonInputPoint(self);
}
/* 026204F8 */
hdBatonCompletionPoint_c* hdBatonCompletionPoint_c::construct(hdBatonCompletionPoint_c* self) {
    if (self == NULL) {
        self = allocateBatonInputPoint(sizeof(hdBatonCompletionPoint_c));
        if (self == NULL) return self;
    }
    constructBatonInputMessageBase(self);
    self->mVtable = &sBatonInputScreenVtable;
    self->mStateHolder.mHolderVtable = &sBatonInputHolderVtable;
    self->mSecondaryVtable = &sBatonInputSecondaryVtable;
    return self;
}
/* 02620564 */
u32 hdBatonCompletionPoint_c::load(u32 first, u32 second, void* descriptor) {
    return loadBatonInputMessageBase(this, &sBatonInputScreenName, first, second, descriptor);
}
/* 0262057C */
void hdBatonCompletionPoint_c::finish() {
    hdAnimSet_play(mpLayout->animSet(), 1, 0, 1.0f);
}
/* 02620598 */
u32 hdBatonCompletionPoint_c::createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = allocateBatonInputLayout(sizeof(hdPartsLayout_c), sBatonInputUiSystem->mHeap.get(), 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    if (!layout->vLoad(resource, first, second, third, 2, 1, descriptor)) return 0;
    u32 groupIndex = sBatonInputAnimationGroupIndex;
    bindBatonInputAnimation(mpLayout->animSet(), 1, &sBatonInputAnimationNames.mEntries[0], &sBatonInputAnimationGroups.mEntries[groupIndex]);
    return 1;
}
/* 02620684 */
static void initializeBatonInputPointStatics() {
    sBatonInputHeaderWords.mWords[2] = 0;
    sBatonInputHeaderWords.mWords[0] = 0;
    sBatonInputHeaderWords.mWords[3] = 0;
    sBatonInputHeaderWords.mWords[1] = 0;
    registerBatonInputGlobal(&sBatonInputHeaderRegistration);
    sBatonInputMinimumRotation = -3.1415927410125732f;
    sBatonInputMaximumRotation = 3.1415927410125732f;
    initializeBatonRotation(&sBatonInputRotationGuard);
    registerBatonInputGlobal(&sBatonInputRotationRegistration);
    initializePositiveBatonRotation(&sBatonInputPositiveRotationGuard);
    registerBatonInputGlobal(&sBatonInputPositiveRotationRegistration);
    sBatonInputAnimationGroups.mEntries[0].mVtable = &sBatonInputStringVtable;
    sBatonInputAnimationGroups.mEntries[0].mText.v = gabi::guest_addr("G_All_00");
    sBatonInputAnimationNames.mEntries[0].mVtable = &sBatonInputStringVtable;
    sBatonInputAnimationNames.mEntries[0].mText.v = gabi::guest_addr("SignFinish");
    sBatonInputScreenName.mVtable = &sBatonInputStringVtable;
    sBatonInputScreenName.mText.v = gabi::guest_addr("BatonInputPoint_01");
}

/* WWHD glue
data sBatonInputHeaderWords 1048E5D4
data sBatonInputHeaderRegistration 101F51DC
data sBatonInputRotationRegistration 101F51E8
data sBatonInputPositiveRotationRegistration 101F51F4
data sBatonInputMinimumRotation 1048E5C8
data sBatonInputMaximumRotation 1048E5CC
data sBatonInputRotationGuard 1048E5D0
data sBatonInputPositiveRotationGuard 1048E5D1
data sBatonInputAnimationGroups 1048E5B8
data sBatonInputAnimationNames 1048E5C0
data sBatonInputScreenName 1048E5E4
data sBatonInputAnimationGroupIndex 100E3A84
data sBatonInputUiSystem 1018C404
str 100E3AAC "G_All_00" in 02620684
str 100E3AB8 "SignFinish" in 02620684
str 100E3AC4 "BatonInputPoint_01" in 02620684
ext 02702574 void destroyBatonInputMessageBase(OneTimePart_l* self, s32 flags)
ext 027025E0 void constructBatonInputMessageBase(OneTimePart_l* self)
ext 0273AD10 hdBatonCompletionPoint_c* allocateBatonInputPoint(u32 size)
ext 0273AF40 void deleteBatonInputPoint(void* object)
ext 02702674 u32 loadBatonInputMessageBase(OneTimePart_l* self, hdUiSafeString* name, u32 first, u32 second, void* descriptor)
ext 0273B050 hdPartsLayout_c* allocateBatonInputLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02004E04 void bindBatonInputAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group)
ext 028F026C void registerBatonInputGlobal(hdBatonInputRegistration_c* registration)
ext 028ED6F8 void initializeBatonRotation(be<u8>* guard)
ext 028EAB2C void initializePositiveBatonRotation(be<u8>* guard)
data sBatonInputScreenVtable 100E3AD8
data sBatonInputHolderVtable 100E3BA0
data sBatonInputSecondaryVtable 100E3BB0
data sBatonInputStringVtable 100E3A88
*/

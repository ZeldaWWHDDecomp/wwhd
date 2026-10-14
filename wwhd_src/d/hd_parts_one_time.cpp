// Message part that starts an animation once per trigger.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_one_time.h"
using namespace gabi;
using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic2_l = gptr<be<u8>>;
using PartStatic3_l = gptr<hdScreenUiSystem>;

extern void* oneTimePartAllocate(u32);
extern void oneTimePartDelete(void*);
extern void oneTimePartConstructBase(hdParts_c*);
extern void oneTimePartDestroyBase(hdParts_c*, u32);
extern void* oneTimePartAllocateHeap(u32, void*, u32);
extern hdPartsLayout_c* oneTimePartConstructLayout(hdPartsLayout_c*);
extern void oneTimePartSetAnimation(void*, u32, u32, f32);
extern void oneTimePartStartAnimation(void*, u32, u32, f32);
static PartStatic3_l sPartHeapOwner;

static u8 sOneTimePart_lVTable;
static u8 sOneTimePart_lReceiver;
static u8 sOneTimePart_lState;

static PartStatic0_l sOneTimeAnimation;


extern void oneTimePartRegister(PartRegistration_l*);
extern void oneTimePartConstructPrimaryLock(be<u8>*);
extern void oneTimePartConstructSecondaryLock(be<u8>*);

static PartGlobals_l sOneTimeGlobals;
static PartRegistration_l sOneTimeRegistrationA;
static PartRegistration_l sOneTimeRegistrationB;
static PartRegistration_l sOneTimeRegistrationC;
static PartStatic0_l sOneTimeMinimumAngle;
static PartStatic0_l sOneTimeMaximumAngle;
static hdUiSafeString sOneTimeName;
static u8 sOneTimeNameVTable;

/* 027025E0 */
OneTimePart_l* OneTimePart_l::construct(OneTimePart_l* part) {
    if (!part) part = static_cast<OneTimePart_l*>(oneTimePartAllocate(0x54));
    if (part) {
        oneTimePartConstructBase(part);
        part->mVtable = at<hdPartsVtbl_c>(ea(&sOneTimePart_lVTable));
        part->mStateHolder.mHolderVtable = at<void>(ea(&sOneTimePart_lReceiver));
        part->mSecondaryVtable = at<void>(ea(&sOneTimePart_lState));
        part->mTriggered = 0;
    }
    return part;
}

/* 02702574 */
void OneTimePart_l::destroy(u32 flags) {
    if (this) {
        mStateHolder.mHolderVtable = at<void>(ea(&sOneTimePart_lReceiver));
        mSecondaryVtable = at<void>(ea(&sOneTimePart_lState));
        oneTimePartDestroyBase(this, 0);
        if (flags & 1) oneTimePartDelete(this);
    }
}

/* 02702674 */
u32 OneTimePart_l::create(void* archive, void* resource, u32 mode, u32 flags) {
    if (!call_ptr<u32>(messagePartVTable(this)[0x7C / 4].get(), this, archive, 0u, resource, mode, flags))
        return 0;
    resetAnimation();
    return 1;
}

/* 02702474 */
u32 OneTimePart_l::currentState() {
    return u32(mVisitorWord);
}

/* 02702654 */
void OneTimePart_l::resetAnimation() {
    hdPartsLayout_c* layout = mpLayout.get();
    u32 animation = sOneTimeAnimation;
    oneTimePartSetAnimation(layout->animSet(), 0, animation, 0.0f);
}

/* 027026F8 */
void OneTimePart_l::update() {
    if (mTriggered) {
        hdPartsLayout_c* layout = mpLayout.get();
        mTriggered = 0;
        u32 animation = sOneTimeAnimation;
        oneTimePartStartAnimation(layout->animSet(), 0, animation, 1.0f);
    }
}

/* 027026E8 */
void OneTimePart_l::dispatchUpdate() {
    call_ptr<void>(messagePartVTable(this)[0x84 / 4].get(), this);
}

/* 02702728 */
u32 OneTimePart_l::createLayout(void* archive, void* resource, u32 mode, u32 flags, u32 user) {
    void* heap = sPartHeapOwner.get()->mHeap.get();
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(oneTimePartAllocateHeap(0x110, heap, 4));
    if (layout) layout = oneTimePartConstructLayout(layout);
    mpLayout = layout;
    if (!layout) return 0;
    call_ptr<void>(layout->vtbl()->mLoad.get(), layout, archive, resource, mode, flags, 1u, 1u, user);
    return 1;
}

/* 027027DC */
void initializeOneTimeParts() {
    sOneTimeGlobals.mHead = 0;
    sOneTimeGlobals.mOwner = 0;
    sOneTimeGlobals.mCount = 0;
    sOneTimeGlobals.mTail = 0;
    oneTimePartRegister(&sOneTimeRegistrationA);
    sOneTimeGlobals.mMinimumAngle = sOneTimeMinimumAngle;
    sOneTimeGlobals.mMaximumAngle = sOneTimeMaximumAngle;
    oneTimePartConstructPrimaryLock(&sOneTimeGlobals.mPrimaryLock);
    oneTimePartRegister(&sOneTimeRegistrationB);
    oneTimePartConstructSecondaryLock(&sOneTimeGlobals.mSecondaryLock);
    oneTimePartRegister(&sOneTimeRegistrationC);
    sOneTimeName.mVtable = at<hdUiStringTable>(ea(&sOneTimeNameVTable));
    sOneTimeName.mText.v = gabi::guest_addr("OneTime");
}

/* WWHD glue (tools/verify/autoglue.py)
data sPartHeapOwner 1018C404
data sOneTimePart_lVTable 1010BAD0
data sOneTimePart_lReceiver 1010BB98
data sOneTimePart_lState 1010BBA8
data sOneTimeAnimation 1010BA9C
data sOneTimeGlobals 1049E6F4
data sOneTimeRegistrationA 101F7318
data sOneTimeRegistrationB 101F7324
data sOneTimeRegistrationC 101F7330
data sOneTimeMinimumAngle 1010BAC0
data sOneTimeMaximumAngle 1010BAC4
data sOneTimeName 1049E710
data sOneTimeNameVTable 1010BAA0
ext 0273AD10 void* oneTimePartAllocate(u32)
ext 0273AF40 void oneTimePartDelete(void*)
ext 026F89E8 void oneTimePartConstructBase(hdParts_c*)
ext 026F88F0 void oneTimePartDestroyBase(hdParts_c*,u32)
ext 0273B050 void* oneTimePartAllocateHeap(u32,void*,u32)
ext 026FADB0 hdPartsLayout_c* oneTimePartConstructLayout(hdPartsLayout_c*)
ext 0200552C void oneTimePartSetAnimation(void*,u32,u32,f32)
ext 020053E4 void oneTimePartStartAnimation(void*,u32,u32,f32)
ext 028F026C void oneTimePartRegister(PartRegistration_l*)
ext 028ED6F8 void oneTimePartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void oneTimePartConstructSecondaryLock(be<u8>*)
str 1010BAC8 "OneTime" in 027027DC
*/

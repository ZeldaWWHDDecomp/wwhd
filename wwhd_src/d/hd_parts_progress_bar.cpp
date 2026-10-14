// Message progress bar animation and saved-value updates.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_progress_bar.h"
using namespace gabi;
using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic2_l = gptr<be<u8>>;
using PartStatic3_l = gptr<hdScreenUiSystem>;

extern void* progressBarPartAllocate(u32);
extern void progressBarPartDelete(void*);
extern void progressBarPartConstructBase(hdParts_c*);
extern void progressBarPartDestroyBase(hdParts_c*, u32);
extern void* progressBarPartAllocateHeap(u32, void*, u32);
extern hdPartsLayout_c* progressBarPartConstructLayout(hdPartsLayout_c*);
extern void progressBarPartSetAnimation(void*, u32, u32, f32);
static PartStatic3_l sProgressHeapOwner;

static u8 sProgressBarPart_lVTable;
static u8 sProgressBarPart_lReceiver;
static u8 sProgressBarPart_lState;

struct ProgressAnimationIds_l { be<u32> mIds[2]; };
static ProgressAnimationIds_l sProgressAnimations;
extern void* progressBarPartGetPlay();
static PartStatic2_l sProgressSave;

extern void progressBarPartRegister(PartRegistration_l*);
extern void progressBarPartConstructPrimaryLock(be<u8>*);
extern void progressBarPartConstructSecondaryLock(be<u8>*);

static PartGlobals_l sProgressBarGlobals;
static PartRegistration_l sProgressBarRegistrationA;
static PartRegistration_l sProgressBarRegistrationB;
static PartRegistration_l sProgressBarRegistrationC;
static PartStatic0_l sProgressBarMinimumAngle;
static PartStatic0_l sProgressBarMaximumAngle;
static hdUiSafeString sProgressBarName;
static u8 sProgressBarNameVTable;

static PartStatic0_l sProgressTypeGuard;
static hdUiAdaptorType sProgressType;
static PartStatic0_l sBasicTypeGuard;
static hdUiAdaptorType sBasicType;
static PartStatic0_l sUiTypeGuard;
static hdUiAdaptorType sUiType;
static PartStatic0_l sRootTypeGuard;
static hdUiAdaptorType sRootType;

/* 02702B40 */
ProgressBarPart_l* ProgressBarPart_l::construct(ProgressBarPart_l* part) {
    if (!part) part = static_cast<ProgressBarPart_l*>(progressBarPartAllocate(0x58));
    if (part) {
        progressBarPartConstructBase(part);
        part->mVtable = at<hdPartsVtbl_c>(ea(&sProgressBarPart_lVTable));
        part->mStateHolder.mHolderVtable = at<void>(ea(&sProgressBarPart_lReceiver));
        part->mSecondaryVtable = at<void>(ea(&sProgressBarPart_lState));
        part->mShown = 0;
        part->mTarget = 0;
    }
    return part;
}

/* 02702AD4 */
void ProgressBarPart_l::destroy(u32 flags) {
    if (this) {
        mStateHolder.mHolderVtable = at<void>(ea(&sProgressBarPart_lReceiver));
        mSecondaryVtable = at<void>(ea(&sProgressBarPart_lState));
        progressBarPartDestroyBase(this, 0);
        if (flags & 1) progressBarPartDelete(this);
    }
}

/* 02702CCC */
u32 ProgressBarPart_l::create(void* archive, void* resource, u32 mode, u32 flags) {
    if (!call_ptr<u32>(messagePartVTable(this)[0x7C / 4].get(), this, archive, 0u, resource, mode, flags))
        return 0;
    loadProgress();
    return 1;
}

/* 02702BB8 */
void ProgressBarPart_l::refresh() {
    u32 target = mTarget;
    u32 shown = mShown;
    u32 animation = 0;
    u32 maximum = 16;
    if (target > 16) { animation = 1; maximum = 32; }
    if (shown > maximum) shown = maximum;
    hdPartsLayout_c* layout = mpLayout.get();
    u32 resource = sProgressAnimations.mIds[animation];
    progressBarPartSetAnimation(layout->animSet(), animation, resource, f32(shown));
}

/* 02702C80 */
void ProgressBarPart_l::loadProgress() {
    mTarget = sProgressSave.get()[0x33];
    void* play = progressBarPartGetPlay();
    mShown = reinterpret_cast<ProgressPlay_l*>(play)->mShown;
    refresh();
}

/* 02702D40 */
void ProgressBarPart_l::dispatchUpdate() {
    call_ptr<void>(messagePartVTable(this)[0x84 / 4].get(), this);
}

/* 02702E08 */
void ProgressBarPart_l::reset() {
    mTarget = 0;
    mShown = 0;
}

/* 02702E18 */
u32 ProgressBarPart_l::createLayout(void* archive, void* resource, u32 mode, u32 flags, u32 user) {
    void* heap = sProgressHeapOwner.get()->mHeap.get();
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(progressBarPartAllocateHeap(0x110, heap, 4));
    if (layout) layout = progressBarPartConstructLayout(layout);
    mpLayout = layout;
    if (!layout) return 0;
    call_ptr<void>(layout->vtbl()->mLoad.get(), layout, archive, resource, mode, flags, 2u, 1u, user);
    return 1;
}

/* 02702ECC */
void initializeProgressBarParts() {
    sProgressBarGlobals.mHead = 0;
    sProgressBarGlobals.mOwner = 0;
    sProgressBarGlobals.mCount = 0;
    sProgressBarGlobals.mTail = 0;
    progressBarPartRegister(&sProgressBarRegistrationA);
    sProgressBarGlobals.mMinimumAngle = sProgressBarMinimumAngle;
    sProgressBarGlobals.mMaximumAngle = sProgressBarMaximumAngle;
    progressBarPartConstructPrimaryLock(&sProgressBarGlobals.mPrimaryLock);
    progressBarPartRegister(&sProgressBarRegistrationB);
    progressBarPartConstructSecondaryLock(&sProgressBarGlobals.mSecondaryLock);
    progressBarPartRegister(&sProgressBarRegistrationC);
    sProgressBarName.mVtable = at<hdUiStringTable>(ea(&sProgressBarNameVTable));
    sProgressBarName.mText.v = gabi::guest_addr("ProgressBar");
}

/* 02702A20 */
hdUiAdaptorType* progressBarTypeInfo() {
 if (!sProgressTypeGuard) {
  u32 basicInitialized = sBasicTypeGuard;
  sProgressTypeGuard = 1;
  if (!basicInitialized) {
   u32 uiInitialized = sUiTypeGuard;
   sBasicTypeGuard = 1;
   if (!uiInitialized) {
    u32 rootInitialized = sRootTypeGuard;
    sUiTypeGuard = 1;
    if (!rootInitialized) {
     sRootTypeGuard = 1;
     sRootType.mParent = nullptr;
    }
    sBasicType.mParent = &sUiType;
    sUiType.mParent = &sRootType;
   } else {
    sBasicType.mParent = &sUiType;
   }
  }
  sProgressType.mParent = &sBasicType;
 }
 return &sProgressType;
}

/* WWHD glue (tools/verify/autoglue.py)
data sProgressHeapOwner 1018C404
data sProgressBarPart_lVTable 1010BCB8
data sProgressBarPart_lReceiver 1010BD80
data sProgressBarPart_lState 1010BD90
data sProgressAnimations 1010BC70
data sProgressSave 101F84DC
data sProgressBarGlobals 1049E734
data sProgressBarRegistrationA 101F7360
data sProgressBarRegistrationB 101F736C
data sProgressBarRegistrationC 101F7378
data sProgressBarMinimumAngle 1010BCA0
data sProgressBarMaximumAngle 1010BCA4
data sProgressBarName 1049E750
data sProgressBarNameVTable 1010BC7C
data sProgressTypeGuard 101FDDA0
data sProgressType 101FDDA4
data sBasicTypeGuard 101FD914
data sBasicType 101FD918
data sUiTypeGuard 101FD890
data sUiType 101FD91C
data sRootTypeGuard 101FD888
data sRootType 101FD8FC
ext 0273AD10 void* progressBarPartAllocate(u32)
ext 0273AF40 void progressBarPartDelete(void*)
ext 026F89E8 void progressBarPartConstructBase(hdParts_c*)
ext 026F88F0 void progressBarPartDestroyBase(hdParts_c*,u32)
ext 0273B050 void* progressBarPartAllocateHeap(u32,void*,u32)
ext 026FADB0 hdPartsLayout_c* progressBarPartConstructLayout(hdPartsLayout_c*)
ext 0200552C void progressBarPartSetAnimation(void*,u32,u32,f32)
ext 025200D4 void* progressBarPartGetPlay()
ext 028F026C void progressBarPartRegister(PartRegistration_l*)
ext 028ED6F8 void progressBarPartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void progressBarPartConstructSecondaryLock(be<u8>*)
str 1010BCA8 "ProgressBar" in 02702ECC
*/

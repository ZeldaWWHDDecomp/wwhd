// Message layout owner construction and lifetime.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_layout_owner.h"
using namespace gabi;
using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic2_l = gptr<be<u8>>;
using PartStatic3_l = gptr<hdScreenUiSystem>;

extern void* layoutOwnerPartPartAllocate(u32);
extern void layoutOwnerPartPartDelete(void*);


extern void layoutOwnerPartPartRegister(PartRegistration_l*);
extern void layoutOwnerPartPartConstructPrimaryLock(be<u8>*);
extern void layoutOwnerPartPartConstructSecondaryLock(be<u8>*);

static PartGlobals_l sLayoutOwnerGlobals;
static PartRegistration_l sLayoutOwnerRegistrationA;
static PartRegistration_l sLayoutOwnerRegistrationB;
static PartRegistration_l sLayoutOwnerRegistrationC;
static PartStatic0_l sLayoutOwnerMinimumAngle;
static PartStatic0_l sLayoutOwnerMaximumAngle;

static u8 sLayoutOwnerVTable;
extern void layoutOwnerPartLayoutOwnerConstruct(hdMessageLayoutOwner_c*, void*, u32, u32);
extern void layoutOwnerPartLayoutOwnerDestroy(hdMessageLayoutOwner_c*, u32);

/* 0270298C */
void initializeLayoutOwnerParts() {
    sLayoutOwnerGlobals.mHead = 0;
    sLayoutOwnerGlobals.mOwner = 0;
    sLayoutOwnerGlobals.mCount = 0;
    sLayoutOwnerGlobals.mTail = 0;
    layoutOwnerPartPartRegister(&sLayoutOwnerRegistrationA);
    sLayoutOwnerGlobals.mMinimumAngle = sLayoutOwnerMinimumAngle;
    sLayoutOwnerGlobals.mMaximumAngle = sLayoutOwnerMaximumAngle;
    layoutOwnerPartPartConstructPrimaryLock(&sLayoutOwnerGlobals.mPrimaryLock);
    layoutOwnerPartPartRegister(&sLayoutOwnerRegistrationB);
    layoutOwnerPartPartConstructSecondaryLock(&sLayoutOwnerGlobals.mSecondaryLock);
    layoutOwnerPartPartRegister(&sLayoutOwnerRegistrationC);
}

/* 027028A8 */
hdMessageLayoutOwner_c* hdMessageLayoutOwner_c::construct(hdMessageLayoutOwner_c* part, void* layout, u32 mode, u32 flags) {
 if (!part) part = static_cast<hdMessageLayoutOwner_c*>(layoutOwnerPartPartAllocate(0xB4));
 if (part) {
  layoutOwnerPartLayoutOwnerConstruct(part, layout, mode, flags);
  part->mFinished = 1;
  part->mPane.mVtable = at<hdPartsPaneVtbl_c>(ea(&sLayoutOwnerVTable));
  part->mAnimated = 1;
 }
 return part;
}

/* 02702938 */
void hdMessageLayoutOwner_c::destroy(u32 flags) {
 if (this) {
  layoutOwnerPartLayoutOwnerDestroy(this, 0);
  if (flags & 1) layoutOwnerPartPartDelete(this);
 }
}

/* WWHD glue (tools/verify/autoglue.py)
data sLayoutOwnerGlobals 1049E718
data sLayoutOwnerRegistrationA 101F733C
data sLayoutOwnerRegistrationB 101F7348
data sLayoutOwnerRegistrationC 101F7354
data sLayoutOwnerMinimumAngle 1010BBB8
data sLayoutOwnerMaximumAngle 1010BBBC
data sLayoutOwnerVTable 1010BBC0
ext 0273AD10 void* layoutOwnerPartPartAllocate(u32)
ext 0273AF40 void layoutOwnerPartPartDelete(void*)
ext 028F026C void layoutOwnerPartPartRegister(PartRegistration_l*)
ext 028ED6F8 void layoutOwnerPartPartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void layoutOwnerPartPartConstructSecondaryLock(be<u8>*)
ext 020037D0 void layoutOwnerPartLayoutOwnerConstruct(hdMessageLayoutOwner_c*,void*,u32,u32)
ext 02003854 void layoutOwnerPartLayoutOwnerDestroy(hdMessageLayoutOwner_c*,u32)
*/

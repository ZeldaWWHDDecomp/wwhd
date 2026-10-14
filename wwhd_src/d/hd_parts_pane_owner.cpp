// Message pane owner construction and lifetime.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_pane_owner.h"
using namespace gabi;
using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic2_l = gptr<be<u8>>;
using PartStatic3_l = gptr<hdScreenUiSystem>;

extern void* paneOwnerPartPartAllocate(u32);
extern void paneOwnerPartPartDelete(void*);


extern void paneOwnerPartPartRegister(PartRegistration_l*);
extern void paneOwnerPartPartConstructPrimaryLock(be<u8>*);
extern void paneOwnerPartPartConstructSecondaryLock(be<u8>*);

static PartGlobals_l sPaneOwnerGlobals;
static PartRegistration_l sPaneOwnerRegistrationA;
static PartRegistration_l sPaneOwnerRegistrationB;
static PartRegistration_l sPaneOwnerRegistrationC;
static PartStatic0_l sPaneOwnerMinimumAngle;
static PartStatic0_l sPaneOwnerMaximumAngle;

static u8 sPaneOwnerVTable;
static PartStatic1_l sDefaultPane;
extern void paneOwnerPartPaneOwnerConstruct(PaneOwnerPart_l*);
extern void paneOwnerPartPaneOwnerDestroy(PaneOwnerPart_l*, u32);
extern void paneOwnerPartPaneOwnerRelease(void*);

/* 02703068 */
void initializePaneOwnerParts() {
    sPaneOwnerGlobals.mHead = 0;
    sPaneOwnerGlobals.mOwner = 0;
    sPaneOwnerGlobals.mCount = 0;
    sPaneOwnerGlobals.mTail = 0;
    paneOwnerPartPartRegister(&sPaneOwnerRegistrationA);
    sPaneOwnerGlobals.mMinimumAngle = sPaneOwnerMinimumAngle;
    sPaneOwnerGlobals.mMaximumAngle = sPaneOwnerMaximumAngle;
    paneOwnerPartPartConstructPrimaryLock(&sPaneOwnerGlobals.mPrimaryLock);
    paneOwnerPartPartRegister(&sPaneOwnerRegistrationB);
    paneOwnerPartPartConstructSecondaryLock(&sPaneOwnerGlobals.mSecondaryLock);
    paneOwnerPartPartRegister(&sPaneOwnerRegistrationC);
}

/* 02702F98 */
PaneOwnerPart_l* PaneOwnerPart_l::construct(PaneOwnerPart_l* part) {
 if (!part) part = static_cast<PaneOwnerPart_l*>(paneOwnerPartPartAllocate(0x2C));
 if (part) {
  paneOwnerPartPaneOwnerConstruct(part);
  part->mpVTable = at<gfn>(ea(&sPaneOwnerVTable));
  part->mpPane = nullptr;
  part->mpPane = sDefaultPane.get();
 }
 return part;
}

/* 02703000 */
void PaneOwnerPart_l::destroy(u32 flags) {
 if (this) {
  void* pane = mpPane.get();
  mpVTable = at<gfn>(ea(&sPaneOwnerVTable));
  paneOwnerPartPaneOwnerRelease(pane);
  paneOwnerPartPaneOwnerDestroy(this, 0);
  if (flags & 1) paneOwnerPartPartDelete(this);
 }
}

/* WWHD glue (tools/verify/autoglue.py)
data sPaneOwnerGlobals 1049E758
data sPaneOwnerRegistrationA 101F7384
data sPaneOwnerRegistrationB 101F7390
data sPaneOwnerRegistrationC 101F739C
data sPaneOwnerMinimumAngle 1010BDA0
data sPaneOwnerMaximumAngle 1010BDA4
data sPaneOwnerVTable 1010BDA8
data sDefaultPane 101FC8B4
ext 0273AD10 void* paneOwnerPartPartAllocate(u32)
ext 0273AF40 void paneOwnerPartPartDelete(void*)
ext 028F026C void paneOwnerPartPartRegister(PartRegistration_l*)
ext 028ED6F8 void paneOwnerPartPartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void paneOwnerPartPartConstructSecondaryLock(be<u8>*)
ext 02873470 void paneOwnerPartPaneOwnerConstruct(PaneOwnerPart_l*)
ext 02873584 void paneOwnerPartPaneOwnerDestroy(PaneOwnerPart_l*,u32)
ext 0287F3D0 void paneOwnerPartPaneOwnerRelease(void*)
*/

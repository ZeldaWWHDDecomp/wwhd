// Message part with no player-control transitions.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_no_control.h"
using namespace gabi;
using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic2_l = gptr<be<u8>>;
using PartStatic3_l = gptr<hdScreenUiSystem>;

extern void* noControlPartAllocate(u32);
extern void noControlPartDelete(void*);
extern void noControlPartConstructBase(hdParts_c*);
extern void noControlPartDestroyBase(hdParts_c*, u32);
static u8 sNoControlPart_lVTable;
static u8 sNoControlPart_lReceiver;
static u8 sNoControlPart_lState;


extern void noControlPartRegister(PartRegistration_l*);
extern void noControlPartConstructPrimaryLock(be<u8>*);
extern void noControlPartConstructSecondaryLock(be<u8>*);
static PartGlobals_l sNoControlGlobals;
static PartRegistration_l sNoControlRegistrationA;
static PartRegistration_l sNoControlRegistrationB;
static PartRegistration_l sNoControlRegistrationC;
static PartStatic0_l sNoControlMinimumAngle;
static PartStatic0_l sNoControlMaximumAngle;
static hdUiSafeString sNoControlName;
static u8 sNoControlNameVTable;

/* 02700ADC */
NoControlPart_l* NoControlPart_l::construct(NoControlPart_l* part) {
    if (!part) part = static_cast<NoControlPart_l*>(noControlPartAllocate(0x50));
    if (part) {
        noControlPartConstructBase(part);
        part->mVtable = at<hdPartsVtbl_c>(ea(&sNoControlPart_lVTable));
        part->mStateHolder.mHolderVtable = at<void>(ea(&sNoControlPart_lReceiver));
        part->mSecondaryVtable = at<void>(ea(&sNoControlPart_lState));
    }
    return part;
}

/* 02700A70 */
void NoControlPart_l::destroy(u32 flags) {
    if (this) {
        mStateHolder.mHolderVtable = at<void>(ea(&sNoControlPart_lReceiver));
        mSecondaryVtable = at<void>(ea(&sNoControlPart_lState));
        noControlPartDestroyBase(this, 0);
        if (flags & 1) noControlPartDelete(this);
    }
}

/* 02700B48 */
u32 NoControlPart_l::create(void* archive, void* resource, u32 mode, u32 flags) {
    if (!call_ptr<u32>(messagePartVTable(this)[0x7C / 4].get(), this, archive, 0u, resource, mode, flags))
        return 0;
    return call_ptr<u32>(messagePartVTable(this)[0xA4 / 4].get(), this) ? 1 : 0;
}

/* 027008F4 */
u32 NoControlPart_l::canControl() {
    return 1;
}

/* 027008FC */
u32 NoControlPart_l::canDecide() {
    return 1;
}

/* 02700918 */
u32 NoControlPart_l::canCancel() {
    return 1;
}

/* 02700924 */
u32 NoControlPart_l::canCursor() {
    return 1;
}

/* 02700934 */
u32 NoControlPart_l::canTouch() {
    return 1;
}

/* 02700940 */
u32 NoControlPart_l::canAdvance() {
    return 1;
}

/* 02700958 */
u32 NoControlPart_l::canClose() {
    return 1;
}

/* 02700960 */
u32 NoControlPart_l::canOpen() {
    return 1;
}

/* 02700968 */
u32 NoControlPart_l::canUpdate() {
    return 1;
}

/* 02700970 */
u32 NoControlPart_l::currentState() {
    return u32(mVisitorWord);
}

/* 02700BD0 */
void initializeNoControlParts() {
    sNoControlGlobals.mHead = 0;
    sNoControlGlobals.mOwner = 0;
    sNoControlGlobals.mCount = 0;
    sNoControlGlobals.mTail = 0;
    noControlPartRegister(&sNoControlRegistrationA);
    sNoControlGlobals.mMinimumAngle = sNoControlMinimumAngle;
    sNoControlGlobals.mMaximumAngle = sNoControlMaximumAngle;
    noControlPartConstructPrimaryLock(&sNoControlGlobals.mPrimaryLock);
    noControlPartRegister(&sNoControlRegistrationB);
    noControlPartConstructSecondaryLock(&sNoControlGlobals.mSecondaryLock);
    noControlPartRegister(&sNoControlRegistrationC);
    sNoControlName.mVtable = at<hdUiStringTable>(ea(&sNoControlNameVTable));
    sNoControlName.mText.v = gabi::guest_addr("NoControl");
}

/* WWHD glue (tools/verify/autoglue.py)
data sNoControlPart_lVTable 1010B610
data sNoControlPart_lReceiver 1010B6D8
data sNoControlPart_lState 1010B6E8
data sNoControlGlobals 1049E4FC
data sNoControlRegistrationA 101F72D0
data sNoControlRegistrationB 101F72DC
data sNoControlRegistrationC 101F72E8
data sNoControlMinimumAngle 1010B5FC
data sNoControlMaximumAngle 1010B600
data sNoControlName 1049E518
data sNoControlNameVTable 1010B5DC
ext 0273AD10 void* noControlPartAllocate(u32)
ext 0273AF40 void noControlPartDelete(void*)
ext 026F89E8 void noControlPartConstructBase(hdParts_c*)
ext 026F88F0 void noControlPartDestroyBase(hdParts_c*,u32)
ext 028F026C void noControlPartRegister(PartRegistration_l*)
ext 028ED6F8 void noControlPartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void noControlPartConstructSecondaryLock(be<u8>*)
str 1010B604 "NoControl" in 02700BD0
*/

/* HD Heart_00: per-screen Heart part and its layout names. */
#include "d/hd_screen_heart_part.h"
static u32 sHeartPartHeartVtable;
static u32 sHeartPartHolderVtable;
static u32 sHeartPartInterfaceVtable;
static hdHeartInitLists_l sHeartPartInitLists;
static hdHeartInitAngles_l sHeartPartInitAngles;
static hdHeartInitRecord_l sHeartPartFirstRecord;
static hdHeartInitRecord_l sHeartPartSecondRecord;
static hdHeartInitRecord_l sHeartPartThirdRecord;
static hdHeartAngleLimits_l sHeartPartAngleLimits;
static u32 sHeartPartStringVtable;
static hdUiSafeString sHeartPartLoopName;
static hdUiSafeString sHeartPartAllName;
static hdUiSafeString sHeartPartScreenName;
void heartRegisterStatic(hdHeartInitRecord_l*);
void heartInitFlag(be<u8>*);
void heartInitByte(be<u8>*);

/* 0266D07C */
void hdHudHeartPart_c::destroy(hdHudHeartPart_c* part, u32 flags) {
    if (!part) return;
    part->mStateHolder.mHolderVtable = &sHeartPartHolderVtable;
    part->mSecondaryVtable = &sHeartPartInterfaceVtable;
    hdHeartPart_c::destroy(part, 0);
    if (flags & 1) operator_delete(part);
}
/* 0266D0E8 */
hdHudHeartPart_c* hdHudHeartPart_c::construct(hdHudHeartPart_c* part) {
    if (!part) {
        part = static_cast<hdHudHeartPart_c*>(operator_new(sizeof(hdHudHeartPart_c)));
        if (!part) return NULL;
    }
    hdPartsHeart_construct(part);
    part->mVtable.v = gabi::ea(&sHeartPartHeartVtable);
    part->mStateHolder.mHolderVtable = &sHeartPartHolderVtable;
    part->mSecondaryVtable = &sHeartPartInterfaceVtable;
    return part;
}
/* 0266D154 */
BOOL hdHudHeartPart_c::loadScreen(void* archive, void* heap, void* parent) {
    return hdHeartPart_c::load(&sHeartPartScreenName, archive, heap, parent);
}
/* 0266D16C */
static void initHudHeartPart() {
    sHeartPartInitLists.mFourth = 0;
    sHeartPartInitLists.mThird = 0;
    sHeartPartInitLists.mSecond = 0;
    sHeartPartInitLists.mFirst = 0;
    heartRegisterStatic(&sHeartPartFirstRecord);
    f32 minimum = sHeartPartAngleLimits.mMinimum;
    f32 maximum = sHeartPartAngleLimits.mMaximum;
    sHeartPartInitAngles.mMinimum = minimum;
    sHeartPartInitAngles.mMaximum = maximum;
    heartInitFlag(&sHeartPartInitAngles.mFlag);
    heartRegisterStatic(&sHeartPartSecondRecord);
    heartInitByte(&sHeartPartInitAngles.mByte);
    heartRegisterStatic(&sHeartPartThirdRecord);
    sHeartPartLoopName.mVtable.v = gabi::ea(&sHeartPartStringVtable);
    sHeartPartLoopName.mText.v = gabi::guest_addr("G_Loop_00");
    sHeartPartAllName.mText.v = gabi::guest_addr("G_All_00");
    sHeartPartAllName.mVtable.v = gabi::ea(&sHeartPartStringVtable);
    sHeartPartScreenName.mVtable.v = gabi::ea(&sHeartPartStringVtable);
    sHeartPartScreenName.mText.v = gabi::guest_addr("Heart_00");
}
/* WWHD glue
 data sHeartPartHeartVtable 100F5490
 data sHeartPartHolderVtable 100F55A0
 data sHeartPartInterfaceVtable 100F55B0
 data sHeartPartInitLists 104945C8
 data sHeartPartInitAngles 104945BC
 data sHeartPartAngleLimits 100F5464
 data sHeartPartFirstRecord 101F5FB0
 data sHeartPartSecondRecord 101F5FBC
 data sHeartPartThirdRecord 101F5FC8
 data sHeartPartStringVtable 100F5444
 data sHeartPartLoopName 104945D8
 data sHeartPartAllName 104945E0
 data sHeartPartScreenName 104945E8
 str 100F546C "G_Loop_00" in 0266D16C
 str 100F5478 "G_All_00" in 0266D16C
 str 100F5484 "Heart_00" in 0266D16C
 ext 026F6BC8 void hdHeartPart_c::destroy(hdHeartPart_c*,u32)
 ext 026F6F48 BOOL hdHeartPart_c::load(void*,void*,void*,void*)
 ext 028F026C void heartRegisterStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void heartInitFlag(be<u8>*)
 ext 028EAB2C void heartInitByte(be<u8>*)
*/

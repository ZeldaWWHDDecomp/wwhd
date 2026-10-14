/* HD LoadingIcon_00: looping loading indicator and its pane names. */
#include "d/hd_screen_loading_icon.h"
static u32 sLoadingIconHeartVtable;
static u32 sLoadingIconHolderVtable;
static u32 sLoadingIconInterfaceVtable;
static hdHeartInitLists_l sLoadingIconInitLists;
static hdHeartInitAngles_l sLoadingIconInitAngles;
static hdHeartInitRecord_l sLoadingIconFirstRecord;
static hdHeartInitRecord_l sLoadingIconSecondRecord;
static hdHeartInitRecord_l sLoadingIconThirdRecord;
static hdHeartAngleLimits_l sLoadingIconAngleLimits;
static u32 sLoadingIconStringVtable;
static hdUiSafeString sLoadingIconLoopName;
static hdUiSafeString sLoadingIconAllName;
static hdUiSafeString sLoadingIconScreenName;
static hdUiSafeString sLoadingIconRootName;
static hdUiSafeString sLoadingIconFirstPane;
static hdUiSafeString sLoadingIconSecondPane;
void loadingRegisterStatic(hdHeartInitRecord_l*);
void loadingInitFlag(be<u8>*);
void loadingInitByte(be<u8>*);

/* 026742B4 */
void hdHudLoadingIcon_c::destroy(hdHudLoadingIcon_c* part, u32 flags) {
    if (!part) return;
    part->mStateHolder.mHolderVtable = &sLoadingIconHolderVtable;
    part->mSecondaryVtable = &sLoadingIconInterfaceVtable;
    hdPartsInOutLoop_c::destroy(part, 0);
    if (flags & 1) operator_delete(part);
}
/* 02674320 */
hdHudLoadingIcon_c* hdHudLoadingIcon_c::construct(hdHudLoadingIcon_c* part) {
    if (!part) {
        part = static_cast<hdHudLoadingIcon_c*>(operator_new(sizeof(hdHudLoadingIcon_c)));
        if (!part) return NULL;
    }
    hdPartsInOutLoop_c::construct(part);
    part->mVtable.v = gabi::ea(&sLoadingIconHeartVtable);
    part->mStateHolder.mHolderVtable = &sLoadingIconHolderVtable;
    part->mSecondaryVtable = &sLoadingIconInterfaceVtable;
    return part;
}
/* 0267438C */
BOOL hdHudLoadingIcon_c::loadScreen(void* archive, void* heap, void* parent) {
    return hdPartsInOutLoop_c::init(&sLoadingIconScreenName,gabi::ea(archive),gabi::ea(heap),static_cast<hdPartsDesc_c*>(parent));
}
/* 026743A4 */
static void initHudLoadingIcon() {
    sLoadingIconInitLists.mFourth = 0;
    sLoadingIconInitLists.mThird = 0;
    sLoadingIconInitLists.mSecond = 0;
    sLoadingIconInitLists.mFirst = 0;
    loadingRegisterStatic(&sLoadingIconFirstRecord);
    f32 minimum = sLoadingIconAngleLimits.mMinimum;
    f32 maximum = sLoadingIconAngleLimits.mMaximum;
    sLoadingIconInitAngles.mMinimum = minimum;
    sLoadingIconInitAngles.mMaximum = maximum;
    loadingInitFlag(&sLoadingIconInitAngles.mFlag);
    loadingRegisterStatic(&sLoadingIconSecondRecord);
    loadingInitByte(&sLoadingIconInitAngles.mByte);
    loadingRegisterStatic(&sLoadingIconThirdRecord);
    sLoadingIconLoopName.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconAllName.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconAllName.mText.v=gabi::guest_addr("G_All_00");
    sLoadingIconRootName.mText.v=gabi::guest_addr("N_All_00");
    sLoadingIconRootName.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconSecondPane.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconSecondPane.mText.v=gabi::guest_addr("P_KeyIcon_01");
    sLoadingIconFirstPane.mText.v=gabi::guest_addr("P_KeyIcon_00");
    sLoadingIconLoopName.mText.v=gabi::guest_addr("G_Loop_00");
    sLoadingIconFirstPane.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconScreenName.mVtable.v=gabi::ea(&sLoadingIconStringVtable);
    sLoadingIconScreenName.mText.v=gabi::guest_addr("LoadingIcon_00");
}
/* WWHD glue
 data sLoadingIconHeartVtable 100F7000
 data sLoadingIconHolderVtable 100F7128
 data sLoadingIconInterfaceVtable 100F7138
 data sLoadingIconInitLists 10495000
 data sLoadingIconInitAngles 10494FF4
 data sLoadingIconAngleLimits 100F6FA4
 data sLoadingIconFirstRecord 101F614C
 data sLoadingIconSecondRecord 101F6158
 data sLoadingIconThirdRecord 101F6164
 data sLoadingIconStringVtable 100F6F84
 data sLoadingIconLoopName 10495010
 data sLoadingIconAllName 10495018
 data sLoadingIconScreenName 10495030
 data sLoadingIconRootName 10494FEC
 data sLoadingIconFirstPane 10495020
 data sLoadingIconSecondPane 10495028
 str 100F6FC4 "N_All_00" in 026743A4
 str 100F6FD0 "P_KeyIcon_00" in 026743A4
 str 100F6FE0 "P_KeyIcon_01" in 026743A4
 str 100F6FAC "G_Loop_00" in 026743A4
 str 100F6FB8 "G_All_00" in 026743A4
 str 100F6FF0 "LoadingIcon_00" in 026743A4
 ext 026F7520 void hdPartsInOutLoop_c::destroy(hdPartsInOutLoop_c*,s32)
 ext 028F026C void loadingRegisterStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void loadingInitFlag(be<u8>*)
 ext 028EAB2C void loadingInitByte(be<u8>*)
 ext 026F7754 hdPartsInOutLoop_c* hdPartsInOutLoop_c::construct(hdPartsInOutLoop_c*)
 ext 026F7844 u32 hdPartsInOutLoop_c::init(void*,u32,u32,hdPartsDesc_c*)
*/

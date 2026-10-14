/** HD pictograph post-tag button and decorative pane discovery; private source. */
#include "d/hd_screen_pictograph_post.h"

void hdPictoDestroyStringsPostPanes(void*, u32, u32, u32, u32, u32);
void hdPictoRegisterGlobalPostPanes(void*);
void hdPictoBasicButtonDestroyPostPanes(hdBasicButton_c*, s32);
struct hdPostButtonNames_l { hdUiSafeString mNames[2]; };
struct hdPostTextNames_l { hdUiSafeString mNames[5]; };
struct hdPostMarkNames_l { hdUiSafeString mNames[2]; };
struct hdPostRegistration_l { u8 mBytes[12]; };
static hdPartsVtbl_c sPostButtonVtable;
static u32 sPostHolderVtable;
static u32 sPostSecondaryVtable;
static hdUiStringTable sPostStringVtablePostPanes;
static hdPostButtonNames_l sPostButtonNames;
static hdPostTextNames_l sPostTextNamesPostPanes;
static hdPostMarkNames_l sPostMarkNamesPostPanes;
static u32 sPostButtonNamesReady;
static hdPostRegistration_l sPostButtonNamesRegistration;

/* 02642798 */
static void destroyPostButtonNames() { hdPictoDestroyStringsPostPanes(&sPostButtonNames, 2, 8, 0x02644708, 0, 0); }

/* 026427BC */
static void destroyPostTextNames() { hdPictoDestroyStringsPostPanes(&sPostTextNamesPostPanes, 5, 8, 0x02644708, 0, 0); }

/* 026427E0 */
static void destroyPostMarkNames() { hdPictoDestroyStringsPostPanes(&sPostMarkNamesPostPanes, 2, 8, 0x02644708, 0, 0); }

/* 02642804 */
void hdPictographPost_c::destroy(hdPictographPost_c* part, s32 flags) {
    if (part != NULL) {
        part->mButton.mStateHolder.mHolderVtable = &sPostHolderVtable;
        part->mButton.mSecondaryVtable = &sPostSecondaryVtable;
        hdPictoBasicButtonDestroyPostPanes(part, 0);
        if (flags & 1) operator_delete(part);
    }
}

/* 02642870 */
hdPictographPost_c* hdPictographPost_c::construct(hdPictographPost_c* part, hdPictureSlotSummaries_c* summaries, u32 slot) {
    if (part == NULL) {
        part = (hdPictographPost_c*)operator_new(sizeof(hdPictographPost_c));
        if (part == NULL) return NULL;
    }
    hdPartsBasicButton_construct(&part->mButton);
    part->mButton.mVtable = &sPostButtonVtable;
    part->mButton.mSecondaryVtable = &sPostSecondaryVtable;
    part->mButton.mStateHolder.mHolderVtable = &sPostHolderVtable;
    if (u32(gabi::ea(part) + offsetof(hdPictographPost_c, mpButtonPanes)) == 0) operator_new(8);
    if (u32(gabi::ea(part) + offsetof(hdPictographPost_c, mpTextPanes)) == 0) operator_new(0x14);
    if (u32(gabi::ea(part) + offsetof(hdPictographPost_c, mpMarkPanes)) == 0) operator_new(8);
    if (u32(gabi::ea(part) + offsetof(hdPictographPost_c, _reservedB0)) == 0) operator_new(0xC);
    part->mSelectedSlot = slot;
    part->mpSummaries = summaries;
    return part;
}

/* 0264293C */
void hdPictographPost_c::initializeButtonPanes() {
    if (sPostButtonNamesReady == 0) {
        sPostButtonNamesReady = 1;
        sPostButtonNames.mNames[0].mVtable = &sPostStringVtablePostPanes;
        sPostButtonNames.mNames[0].mText.v = gabi::guest_addr("N_BtnRot_00");
        sPostButtonNames.mNames[1].mVtable = &sPostStringVtablePostPanes;
        sPostButtonNames.mNames[1].mText.v = gabi::guest_addr("N_Btn_00");
        hdPictoRegisterGlobalPostPanes(&sPostButtonNamesRegistration);
    }
    hdPartsPaneSRT_c* root = hdPictoRoot(&mButton);
    for (u32 i = 0; i < 2; ++i) {
        hdUiSafeString* name = &sPostButtonNames.mNames[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        hdPartsPaneSRT_c* pane = gabi::call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        mpButtonPanes[i] = pane;
    }
}

/* WWHD glue
 data sPostButtonVtable 100EC7A0 nocheck
 data sPostHolderVtable 100EC9F8 nocheck
 data sPostSecondaryVtable 100ECA08 nocheck
 data sPostStringVtablePostPanes 100EC0DC nocheck
 data sPostButtonNames 10490A80 nocheck
 data sPostTextNamesPostPanes 10490A90 nocheck
 data sPostMarkNamesPostPanes 10490AB8 nocheck
 data sPostButtonNamesReady 10490D00 nocheck
 data sPostButtonNamesRegistration 101F5768 nocheck
 str 100EC15C "N_BtnRot_00"
 str 100EC168 "N_Btn_00"
 ext 028F0164 void hdPictoDestroyStringsPostPanes(void*, u32, u32, u32, u32, u32)
 ext 028F026C void hdPictoRegisterGlobalPostPanes(void*)
 ext 026F0B8C void hdPictoBasicButtonDestroyPostPanes(hdBasicButton_c*, s32)
*/

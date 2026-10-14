/** HD post-tag extra mark and player-name line pane discovery; private source. */
#include "d/hd_screen_pictograph_post.h"

void hdPictoRegisterGlobalPostMarks(void*);
struct hdPostMarkNames_l { hdUiSafeString mNames[2]; };
struct hdPostRegistration_l { u8 mBytes[12]; };
static hdPostMarkNames_l sPostMarkNamesPostMarks;
static hdUiStringTable sPostStringVtablePostMarks;
static u32 sPostMarkNamesReady;
static hdPostRegistration_l sPostMarkNamesRegistration;

/* 02642D8C */
void hdPictographPost_c::initializeMarkPanes() {
    if (sPostMarkNamesReady == 0) {
        sPostMarkNamesReady = 1;
        sPostMarkNamesPostMarks.mNames[0].mVtable = &sPostStringVtablePostMarks;
        sPostMarkNamesPostMarks.mNames[0].mText.v = gabi::guest_addr("P_ExtraMark_00");
        sPostMarkNamesPostMarks.mNames[1].mVtable = &sPostStringVtablePostMarks;
        sPostMarkNamesPostMarks.mNames[1].mText.v = gabi::guest_addr("P_NameLine_00");
        hdPictoRegisterGlobalPostMarks(&sPostMarkNamesRegistration);
    }
    hdPartsPaneSRT_c* root = hdPictoRoot(&mButton);
    for (u32 i = 0; i < 2; ++i) {
        hdUiSafeString* name = &sPostMarkNamesPostMarks.mNames[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        hdPartsPaneSRT_c* pane = gabi::call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        mpMarkPanes[i] = pane;
    }
}

/* WWHD glue
 data sPostMarkNamesPostMarks 10490AB8 nocheck
 data sPostStringVtablePostMarks 100EC0DC nocheck
 data sPostMarkNamesReady 10490D08 nocheck
 data sPostMarkNamesRegistration 101F5780 nocheck
 str 100EC1C0 "P_ExtraMark_00"
 str 100EC1B0 "P_NameLine_00"
 ext 028F026C void hdPictoRegisterGlobalPostMarks(void*)
*/

/** HD post-tag text-box discovery and runtime type checks; private source. */
#include "d/hd_screen_pictograph_post.h"

void hdPictoRegisterGlobalPostText(void*);
struct hdPostTextNames_l { hdUiSafeString mNames[5]; };
struct hdPostRegistration_l { u8 mBytes[12]; };
static hdPostTextNames_l sPostTextNamesPostText;
static hdUiStringTable sPostStringVtablePostText;
static u32 sPostTextNamesReady;
static hdPostRegistration_l sPostTextNamesRegistration;
static u32 sTextPaneTypeReadyPostText;
static u32 sTextPaneBaseTypeReadyPostText;
static u32 sPaneTypeReadyPostText;
static u32 sRootTypeReadyPostText;
static hdPartsTypeInfo_c sTextPaneTypePostText;
static hdPartsTypeInfo_c sTextPaneBaseTypePostText;
static hdPartsTypeInfo_c sPaneTypePostText;
static hdPartsTypeInfo_c sRootTypePostText;

/* 02642A9C */
void hdPictographPost_c::initializeTextPanes() {
    if (sPostTextNamesReady == 0) {
        sPostTextNamesReady = 1;
        sPostTextNamesPostText.mNames[0].mVtable = &sPostStringVtablePostText;
        sPostTextNamesPostText.mNames[0].mText.v = gabi::guest_addr("T_Name_00");
        sPostTextNamesPostText.mNames[1].mVtable = &sPostStringVtablePostText;
        sPostTextNamesPostText.mNames[1].mText.v = gabi::guest_addr("T_Place_00");
        sPostTextNamesPostText.mNames[2].mVtable = &sPostStringVtablePostText;
        sPostTextNamesPostText.mNames[2].mText.v = gabi::guest_addr("T_Time_00");
        sPostTextNamesPostText.mNames[3].mVtable = &sPostStringVtablePostText;
        sPostTextNamesPostText.mNames[3].mText.v = gabi::guest_addr("T_Text_00");
        sPostTextNamesPostText.mNames[4].mVtable = &sPostStringVtablePostText;
        sPostTextNamesPostText.mNames[4].mText.v = gabi::guest_addr("T_Btn_00");
        hdPictoRegisterGlobalPostText(&sPostTextNamesRegistration);
    }
    hdPartsPaneSRT_c* root = hdPictoRoot(&mButton);
    for (u32 i = 0; i < 5; ++i) {
        hdUiSafeString* name = &sPostTextNamesPostText.mNames[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        hdPartsPaneSRT_c* pane = gabi::call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        if (sTextPaneTypeReadyPostText == 0) {
            sTextPaneTypeReadyPostText = 1;
            if (sTextPaneBaseTypeReadyPostText == 0) {
                sTextPaneBaseTypeReadyPostText = 1;
                if (sPaneTypeReadyPostText == 0) {
                    sPaneTypeReadyPostText = 1;
                    if (sRootTypeReadyPostText == 0) {
                        sRootTypePostText.mParent = NULL;
                        sRootTypeReadyPostText = 1;
                    }
                    sPaneTypePostText.mParent = &sRootTypePostText;
                }
                sTextPaneBaseTypePostText.mParent = &sPaneTypePostText;
            }
            sTextPaneTypePostText.mParent = &sTextPaneBaseTypePostText;
        }
        hdTextPane_c* text = NULL;
        if (pane != NULL) {
            for (hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo(); type != NULL; type = type->mParent) {
                if (gabi::ea(type) == gabi::ea(&sTextPaneTypePostText)) { text = (hdTextPane_c*)pane; break; }
            }
        }
        mpTextPanes[i] = text;
    }
}

/* WWHD glue
 data sPostTextNamesPostText 10490A90 nocheck
 data sPostStringVtablePostText 100EC0DC nocheck
 data sPostTextNamesReady 10490D04 nocheck
 data sPostTextNamesRegistration 101F5774 nocheck
 data sTextPaneTypeReadyPostText 101FDD40 nocheck
 data sTextPaneBaseTypeReadyPostText 101FD8F0 nocheck
 data sPaneTypeReadyPostText 101FD89C nocheck
 data sRootTypeReadyPostText 101FD888 nocheck
 data sTextPaneTypePostText 101FDD3C nocheck
 data sTextPaneBaseTypePostText 101FD8F4 nocheck
 data sPaneTypePostText 101FD8F8 nocheck
 data sRootTypePostText 101FD8FC nocheck
 str 100EC174 "T_Name_00"
 str 100EC198 "T_Place_00"
 str 100EC180 "T_Time_00"
 str 100EC18C "T_Text_00"
 str 100EC1A4 "T_Btn_00"
 ext 028F026C void hdPictoRegisterGlobalPostText(void*)
*/

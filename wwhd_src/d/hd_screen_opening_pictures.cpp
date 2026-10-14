/* HD opening picture pane lookup. */
#include "d/hd_screen_opening_pictures.h"
static u32 sOpeningPictureStringVtable;
static u32 sPicturePanesReady;
static hdUiSafeString sPicturePaneNames[9] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
static u32 sPicturePaneRegistration[3] = {0, 0, 0};

/* 026BF054 */
void hdOpening_screen_c::findPicturePanes() {
    if (!sPicturePanesReady) {
        sPicturePanesReady = 1;
        sPicturePaneNames[0].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[0], "P_BG_00");
        sPicturePaneNames[1].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[1], "P_OpeningPic_05");
        sPicturePaneNames[2].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[2], "P_OpeningPic_04");
        sPicturePaneNames[3].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[3], "P_OpeningPic_03");
        sPicturePaneNames[4].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[4], "P_OpeningPic_02");
        sPicturePaneNames[5].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[5], "P_OpeningPic_01");
        sPicturePaneNames[6].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[6], "P_OpeningPic_00");
        sPicturePaneNames[7].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[7], "P_OpeningPic_TopLine_00");
        sPicturePaneNames[8].mVtable.v = gabi::ea(&sOpeningPictureStringVtable);
        openingSetString(sPicturePaneNames[8], "P_OpeningPic_BottomLine_00");
        openingRegisterPicturePanes(sPicturePaneRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    u32 getText = sPicturePaneNames[0].mVtable->mAssureTermination.get();
    hdOpening_paneVtable_l* paneVtable = hdPartsView<hdOpening_paneVtable_l>(root->mVtable.v);
    gabi::call_ptr<void>(getText, &sPicturePaneNames[0]);
    void* pane = gabi::call_ptr<void*>(paneVtable->mFindPane, root, sPicturePaneNames[0].mText.get(), 1);
    mpBackgroundPane = pane;
}

/* WWHD glue (tools/verify/autoglue.py): opening picture panes
 data sOpeningPictureStringVtable 100FF91C nocheck
 data sPicturePanesReady 1049903C nocheck
 data sPicturePaneNames 10498F24 nocheck
 data sPicturePaneRegistration 101F67C8 nocheck
 str 100FFB0C "P_BG_00"
 str 100FFB14 "P_OpeningPic_05"
 str 100FFB24 "P_OpeningPic_04"
 str 100FFB34 "P_OpeningPic_03"
 str 100FFB44 "P_OpeningPic_02"
 str 100FFB54 "P_OpeningPic_01"
 str 100FFB64 "P_OpeningPic_00"
 str 100FFB74 "P_OpeningPic_TopLine_00"
 str 100FFB8C "P_OpeningPic_BottomLine_00"
 ext 028F026C void openingRegisterPicturePanes(void*)
*/

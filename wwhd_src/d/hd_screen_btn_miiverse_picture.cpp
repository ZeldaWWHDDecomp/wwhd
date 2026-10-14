/* HD-only picture-button dimensions and texture binding. Private decomp-derived source. */
#include "d/hd_screen_btn_miiverse_comment.h"
static u32 sBtnMiiversePicturePiPair;
static u32 sBtnMiiversePictureRegistration;
static u32 sBtnMiiversePictureZeroObject;
static u32 sBtnMiiversePictureNamesRegistration;
static hdPartsVtbl_c sPictureButtonVtable;
static u8 sPictureButtonHolderVtable;
static u8 sPictureButtonSecondaryVtable;
static hdUiStringTable sPictureButtonStringVtable;
static hdUiSafeString sPictureButtonGroup;
static hdUiSafeString sPictureButtonResource;
struct PictureButtonNames { hdUiSafeString mEntries[3]; };
static PictureButtonNames sPictureButtonNames;
static u32 sPictureButtonNamesReady;
struct PictureButtonPaneName { gptr<const char> mName; };
static PictureButtonPaneName sPictureButtonPaneName;
/* 026414B0 */
static void destroyPictureButtonNames() {
    enum { PICTURE_STRING_DESTRUCTOR = 0x026419F0 };
    destroyMiiverseStringArray(&sPictureButtonNames, 3, sizeof(hdUiSafeString), PICTURE_STRING_DESTRUCTOR, 0, 0);
}
/* 026414D4 */
void hdBtnMiiversePicture_c::destroy(hdBtnMiiversePicture_c* button, s32 flags) {
    if (button == NULL) return;
    button->mButton.mStateHolder.mHolderVtable = &sPictureButtonHolderVtable;
    button->mButton.mSecondaryVtable = &sPictureButtonSecondaryVtable;
    hdBasicButton_c::destroy(button, 0);
    if (flags & 1) operator_delete(button);
}
/* 02641540 */
hdBtnMiiversePicture_c* hdBtnMiiversePicture_c::construct(hdBtnMiiversePicture_c* button) {
    if (button == NULL) {
        button = (hdBtnMiiversePicture_c*)operator_new(sizeof(hdBtnMiiversePicture_c));
        if (button == NULL) return NULL;
    }
    hdPartsBasicButton_construct(&button->mButton);
    button->mButton.mVtable = &sPictureButtonVtable;
    button->mButton.mSecondaryVtable = &sPictureButtonSecondaryVtable;
    button->mButton.mStateHolder.mHolderVtable = &sPictureButtonHolderVtable;
    if (gabi::ea(&button->mpDecorations) == 0) operator_new(sizeof(button->mpDecorations));
    if (gabi::ea(&button->mpPicture) == 0) operator_new(sizeof(button->mpPicture));
    return button;
}
/* 02641924 */
void hdBtnMiiversePicture_c::initializeStatics() {
        sinit_header_statics_z(gabi::ea(&sBtnMiiversePicturePiPair), gabi::ea(&sBtnMiiversePictureRegistration), gabi::ea(&sBtnMiiversePictureZeroObject));
    sPictureButtonGroup.mVtable = &sPictureButtonStringVtable;
    sPictureButtonGroup.mText.v = gabi::guest_addr("G_Btn_00");
    sPictureButtonResource.mVtable = &sPictureButtonStringVtable;
    sPictureButtonResource.mText.v = gabi::guest_addr("BtnMiiversePicture_00");
}
/* 026415CC */
u32 hdBtnMiiversePicture_c::init(void* resource, u32 mode, hdPartsDesc_c* desc) {
    if (!hdPartsBasicButton_init(&mButton, &sPictureButtonResource, (u32)gabi::ea(resource), mode, desc)) return 0;
    if (!sPictureButtonNamesReady) {
        sPictureButtonNamesReady = 1;
        sPictureButtonNames.mEntries[0].mVtable = &sPictureButtonStringVtable;
        sPictureButtonNames.mEntries[0].mText.v = gabi::guest_addr("P_BtnShadow_00");
        sPictureButtonNames.mEntries[1].mVtable = &sPictureButtonStringVtable;
        sPictureButtonNames.mEntries[1].mText.v = gabi::guest_addr("P_DummyPicture_00");
        sPictureButtonNames.mEntries[2].mVtable = &sPictureButtonStringVtable;
        sPictureButtonNames.mEntries[2].mText.v = gabi::guest_addr("P_ZoomIcon_00");
        __register_global_object(gabi::ea(&sBtnMiiversePictureNamesRegistration));
    }
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    for (u32 i = 0; i < 3; ++i) {
        hdUiSafeString* name = &sPictureButtonNames.mEntries[i];
        hdMiiversePaneLookupVtbl_l* vtable = root->mVtable;
        name->assureTermination();
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(vtable->mFind.get(), root, name->mText.get(), 1);
        mpDecorations[i] = pane;
    }
    layout = mButton.mpLayout->lyt();
    root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(root->mVtable->mFind.get(), root, sPictureButtonPaneName.mName.get(), 1);
    hdPartsTypeInfo_c* expected = miiversePictureType();
    mpPicture = (hdMiiversePicturePane_l*)castMiiversePane(pane, expected);
    return 1;
}

/* 026418A4 */
f32 hdBtnMiiversePicture_c::getWidth() { return mpPicture->mWidth; }
/* 026418B0 */
f32 hdBtnMiiversePicture_c::getHeight() { return mpPicture->mHeight; }
/* 026418BC */
void hdBtnMiiversePicture_c::setPicture(void* texture) {
    hdPartsLyt_c* layout = mButton.mpLayout->lyt();
    hdMiiversePaneLookup_l* root = (hdMiiversePaneLookup_l*)layout->mpRootPane.get();
    hdMiiversePaneLookup_l* pane = gabi::call_ptr<hdMiiversePaneLookup_l*>(root->mVtable->mFind.get(), root, "P_DummyPicture_00", 1);
    if (pane != NULL) setMiiversePictureTexture(pane, texture, 0, 1);
}
/* WWHD glue
 data sBtnMiiversePicturePiPair 104909C4 nocheck
 data sBtnMiiversePictureRegistration 101F56FC nocheck
 data sBtnMiiversePictureZeroObject 104909E8 nocheck
 data sBtnMiiversePictureNamesRegistration 101F56F0 nocheck
 data sPictureButtonNamesReady 104909F8
 data sPictureButtonPaneName 101F56EC
 str 100EB830 "P_BtnShadow_00"
 str 100EB80C "P_DummyPicture_00" in 026415CC
 str 100EB820 "P_ZoomIcon_00"

 data sPictureButtonVtable 100EB880
 data sPictureButtonHolderVtable 100EBAD8
 data sPictureButtonSecondaryVtable 100EBAE8
 data sPictureButtonStringVtable 100EB7E4
 data sPictureButtonGroup 104909BC
 data sPictureButtonResource 104909FC
 data sPictureButtonNames 104909D0
 str 100EB874 "G_Btn_00" in 02641924
 str 100EB85C "BtnMiiversePicture_00"
 str 100EB840 "P_DummyPicture_00" in 026418BC
 ext 02704F3C void setMiiversePictureTexture(hdMiiversePaneLookup_l*, void*, u32, u32)
*/

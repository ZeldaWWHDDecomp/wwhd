/* HD Picto Box image and left/right sequence pane lookup. */
#include "d/hd_screen_pictograph_box.h"
using namespace gabi;
struct hdPictoPictureNames { hdUiSafeString mNames[3]; };
static u32 sPictureNamesReady;
static hdPictoPictureNames sPictureNames;
static u8 sPictureStringVtable;
static hdPictoRegistration sPictureNamesRegistration;
void pictographRegisterPictureNames(hdPictoRegistration*);
/* 026C482C */
void hdPictographBox::bindPicturePanes() {
    if (!sPictureNamesReady) {
        sPictureNamesReady = 1;
        sPictureNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sPictureStringVtable);
        sPictureNames.mNames[0].mText.v = guest_addr("L_Pictograph_00");
        sPictureNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sPictureStringVtable);
        sPictureNames.mNames[1].mText.v = guest_addr("L_BtnSeqenceL_00");
        sPictureNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sPictureStringVtable);
        sPictureNames.mNames[2].mText.v = guest_addr("L_BtnSeqenceR_00");
        pictographRegisterPictureNames(&sPictureNamesRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index<3; ++index) {
        hdUiSafeString* name = &sPictureNames.mNames[index];
        auto* table = root->srtVtbl();
        name->assureTermination();
        mPicturePanes[index] = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
}
/* WWHD glue
data sPictureNamesReady 10499A5C
data sPictureNames 104999CC
data sPictureStringVtable 10100C1C
data sPictureNamesRegistration 101F68E4
str 10100D44 "L_Pictograph_00" in 026C482C
str 10100D54 "L_BtnSeqenceL_00" in 026C482C
str 10100D68 "L_BtnSeqenceR_00" in 026C482C
ext 028F026C void pictographRegisterPictureNames(hdPictoRegistration*)
*/

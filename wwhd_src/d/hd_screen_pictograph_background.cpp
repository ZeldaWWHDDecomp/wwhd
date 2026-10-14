/* HD Picto Box background-pane lookup. */
#include "d/hd_screen_pictograph_box.h"
using namespace gabi;
static u32 sBackgroundNameReady;
static hdUiSafeString sBackgroundName;
static u8 sBackgroundStringVtable;
static hdPictoRegistration sBackgroundRegistration;
void pictographRegisterBackground(hdPictoRegistration*);
/* 026C4768 */
void hdPictographBox::bindBackground() {
    if (!sBackgroundNameReady) {
        sBackgroundNameReady = 1;
        sBackgroundName.mVtable = reinterpret_cast<hdUiStringTable*>(&sBackgroundStringVtable);
        sBackgroundName.mText.v = guest_addr("P_BgPattern_00");
        pictographRegisterBackground(&sBackgroundRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    auto* table = root->srtVtbl();
    sBackgroundName.assureTermination();
    mpBackground = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, sBackgroundName.mText.get(), 1);
}
/* WWHD glue
data sBackgroundNameReady 10499A58
data sBackgroundName 10499998
data sBackgroundStringVtable 10100C1C
data sBackgroundRegistration 101F68D8
str 10100D34 "P_BgPattern_00" in 026C4768
ext 028F026C void pictographRegisterBackground(hdPictoRegistration*)
*/

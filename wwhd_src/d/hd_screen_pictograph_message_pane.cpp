/* HD Picto Box message pane lookup and runtime type validation. */
#include "d/hd_screen_pictograph_box.h"
using namespace gabi;
static u32 sMessageNameReady;
static hdUiSafeString sMessageName;
static u8 sMessageStringVtable;
static hdPictoRegistration sMessageNameRegistration;
static u32 sMessageTypeReady;
static hdScreenType sMessageType;
static u32 sTextTypeReady;
static hdScreenType sTextType;
static u32 sPartsTypeReady;
static hdScreenType sPartsType;
static u32 sPaneTypeReady;
static hdScreenType sPaneType;
void pictographRegisterMessageName(hdPictoRegistration*);
/* 026C49B8 */
void hdPictographBox::bindMessageText() {
    if (!sMessageNameReady) {
        sMessageNameReady = 1;
        sMessageName.mVtable = reinterpret_cast<hdUiStringTable*>(&sMessageStringVtable);
        sMessageName.mText.v = guest_addr("T_MsgLine1_00");
        pictographRegisterMessageName(&sMessageNameRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    auto* table = root->srtVtbl();
    sMessageName.assureTermination();
    hdPartsPane_c* pane = call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, sMessageName.mText.get(), 1);
    if (!sMessageTypeReady) {
        sMessageTypeReady = 1;
        if (!sTextTypeReady) {
            sTextTypeReady = 1;
            if (!sPartsTypeReady) {
                sPartsTypeReady = 1;
                if (!sPaneTypeReady) {
                    sPaneTypeReady = 1;
                    sPaneType.mParent = NULL;
                }
                sPartsType.mParent = &sPaneType;
            }
            sTextType.mParent = &sPartsType;
        }
        sMessageType.mParent = &sTextType;
    }
    if (pane) {
        hdScreenType* type = pane->getRuntimeTypeInfo();
        while (type && type != &sMessageType) type = type->mParent;
        if (!type) pane = NULL;
    }
    mpMessageText = pane;
}
/* WWHD glue
data sMessageNameReady 10499A60
data sMessageName 104999A0
data sMessageStringVtable 10100C1C
data sMessageNameRegistration 101F68F0
data sMessageTypeReady 101FDD40
data sMessageType 101FDD3C
data sTextTypeReady 101FD8F0
data sTextType 101FD8F4
data sPartsTypeReady 101FD89C
data sPartsType 101FD8F8
data sPaneTypeReady 101FD888
data sPaneType 101FD8FC
str 10100D7C "T_MsgLine1_00" in 026C49B8
ext 028F026C void pictographRegisterMessageName(hdPictoRegistration*)
*/

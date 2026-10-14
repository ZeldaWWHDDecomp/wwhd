/** Telescope zoom-number pane name registration and runtime type filtering. */
#include "d/hd_screen_telescope_scale.h"
typedef be<u32> hdScaleWord_l;
static hdScaleWord_l sScaleNameReady;
static hdScaleWord_l sTextPaneTypeReady;
static hdScaleWord_l sBasePaneTypeReady;
static hdUiSafeString sScalePaneName;
static hdUiStringTable sScaleStringVtable;
struct hdScaleGlobalRecord_l { be<u32> mWords[3]; };
static hdScaleGlobalRecord_l sScaleNameRegistration;
static hdPartsTypeInfo_c sScaleTextPaneType;
static hdPartsTypeInfo_c sScaleBasePaneType;
struct hdScaleFindVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdScaleFindVtbl_l, mFindPane, 0x5C);
void hdScale_registerGlobal(void*);
/* 026E8694 */
void hdTelescopeScaleSetup_c::findScalePane(hdTelescopeHud_c* screen) {
    if (!sScaleNameReady) {
        sScaleNameReady = 1;
        sScalePaneName.mVtable = &sScaleStringVtable;
        sScalePaneName.mText.v = gabi::guest_addr("T_NumScale_00");
        hdScale_registerGlobal(&sScaleNameRegistration);
    }
    hdPartsPane_c* root = screen->mpLayout->lyt()->mpRootPane;
    hdScaleFindVtbl_l* findTable = hdPartsView<hdScaleFindVtbl_l>(gabi::ea(root->mVtable.get()));
    sScalePaneName.assureTermination();
    hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(findTable->mFindPane.get(),root,sScalePaneName.mText.get(),1u);
    if (!sTextPaneTypeReady) {
        sTextPaneTypeReady = 1;
        if (!sBasePaneTypeReady) {
            sBasePaneTypeReady = 1;
            sScaleBasePaneType.mParent = NULL;
        }
        sScaleTextPaneType.mParent = &sScaleBasePaneType;
    }
    if (pane) {
        hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
        while (type) {
            if (type == &sScaleTextPaneType) {
                screen->mpScalePane = pane;
                return;
            }
            type = type->mParent;
        }
    }
    screen->mpScalePane = NULL;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sScaleNameReady 1049C944
 data sTextPaneTypeReady 101FD89C
 data sBasePaneTypeReady 101FD888
 data sScalePaneName 1049C8AC
 data sScaleStringVtable 1010746C
 str 1010759C "T_NumScale_00" in 026E8694
 data sScaleNameRegistration 101F6F4C
 data sScaleTextPaneType 101FD8F8
 data sScaleBasePaneType 101FD8FC
 ext 028F026C void hdScale_registerGlobal(void*)
*/

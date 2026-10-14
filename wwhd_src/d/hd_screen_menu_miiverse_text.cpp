/* Miiverse message text pane lookup and runtime type check. */
#include "d/hd_screen_menu_miiverse_text.h"
static hdUiStringTable sMiiverseStringVtable;
static hdUiSafeString sMiiverseTextName = {0, 0};
static u32 sMiiverseTextReady = 0;
static u32 sMiiverseTextRegistration[3] = {0, 0, 0};
static u32 sMiiverseTextTypeReady = 0;
static u32 sMiiverseTextBaseTypeReady = 0;
static u32 sMiiversePaneTypeReady = 0;
static u32 sMiiverseRootTypeReady = 0;
static hdPartsTypeInfo_c sMiiverseTextType = {0};
static hdPartsTypeInfo_c sMiiverseTextBaseType = {0};
static hdPartsTypeInfo_c sMiiversePaneType = {0};
static hdPartsTypeInfo_c sMiiverseRootType = {0};

/* 026A3DE8 */
void hdMenuMiiverse_c::findTextPane() {
    if (!sMiiverseTextReady) {
        sMiiverseTextReady = 1;
        sMiiverseTextName.mVtable = &sMiiverseStringVtable;
        sMiiverseTextName.mText.v = gabi::guest_addr("T_Msg_00");
        miiverseRegisterStringNames(sMiiverseTextRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    hdPartsPane_c* pane = static_cast<hdPartsPane_c*>(miiverseFindNamedPane(root, &sMiiverseTextName));
    if (!sMiiverseTextTypeReady) {
        sMiiverseTextTypeReady = 1;
        if (!sMiiverseTextBaseTypeReady) {
            sMiiverseTextBaseTypeReady = 1;
            if (!sMiiversePaneTypeReady) {
                sMiiversePaneTypeReady = 1;
                if (!sMiiverseRootTypeReady) {
                    sMiiverseRootTypeReady = 1;
                    sMiiverseRootType.mParent = nullptr;
                }
                sMiiversePaneType.mParent = &sMiiverseRootType;
            }
            sMiiverseTextBaseType.mParent = &sMiiversePaneType;
        }
        sMiiverseTextType.mParent = &sMiiverseTextBaseType;
    }
    if (pane) {
        hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
        while (type && type != &sMiiverseTextType) type = type->mParent;
        if (!type) pane = nullptr;
    }
    mpTextPane = hdPartsView<hdTextPane_c>(gabi::ea(pane));
}

/* WWHD glue (tools/verify/autoglue.py): Miiverse text pane
 data sMiiverseStringVtable 100FBB3C nocheck
 data sMiiverseTextName 10497614 nocheck
 data sMiiverseTextReady 104976CC nocheck
 data sMiiverseTextRegistration 101F63F8 nocheck
 data sMiiverseTextTypeReady 101FDD40 nocheck
 data sMiiverseTextBaseTypeReady 101FD8F0 nocheck
 data sMiiversePaneTypeReady 101FD89C nocheck
 data sMiiverseRootTypeReady 101FD888 nocheck
 data sMiiverseTextType 101FDD3C nocheck
 data sMiiverseTextBaseType 101FD8F4 nocheck
 data sMiiversePaneType 101FD8F8 nocheck
 data sMiiverseRootType 101FD8FC nocheck
 str 100FBDE0 "T_Msg_00"
*/

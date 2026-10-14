/* Miiverse window edge, body and glow pane lookups. */
#include "d/hd_screen_menu_miiverse_window.h"
static hdUiStringTable sMiiverseStringVtable;
static hdUiSafeString sMiiverseWindowNames[3] = {{0, 0}};
static u32 sMiiverseWindowNamesReady = 0;
static u32 sMiiverseWindowRegistration[3] = {0, 0, 0};

/* 026A4090 */
void hdMenuMiiverse_c::findWindowPanes() {
    if (!sMiiverseWindowNamesReady) {
        sMiiverseWindowNamesReady = 1;
        sMiiverseWindowNames[0].mVtable = &sMiiverseStringVtable;
        sMiiverseWindowNames[0].mText.v = gabi::guest_addr("W_BaseEdgeShadow_00");
        sMiiverseWindowNames[1].mVtable = &sMiiverseStringVtable;
        sMiiverseWindowNames[1].mText.v = gabi::guest_addr("W_Base_00");
        sMiiverseWindowNames[2].mVtable = &sMiiverseStringVtable;
        sMiiverseWindowNames[2].mText.v = gabi::guest_addr("W_BaseGlowLine_00");
        miiverseRegisterStringNames(sMiiverseWindowRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 index = 0; index < 3; ++index) {
        mpWindowPanes[index] = miiverseFindNamedPane(root, &sMiiverseWindowNames[index]);
    }
}

/* WWHD glue (tools/verify/autoglue.py): Miiverse window panes
 data sMiiverseStringVtable 100FBB3C nocheck
 data sMiiverseWindowNames 10497660 nocheck
 data sMiiverseWindowNamesReady 104976D4 nocheck
 data sMiiverseWindowRegistration 101F6410 nocheck
 str 100FBDFC "W_BaseEdgeShadow_00"
 str 100FBE10 "W_Base_00"
 str 100FBE1C "W_BaseGlowLine_00"
*/

/* Miiverse background-pattern pane lookup. */
#include "d/hd_screen_menu_miiverse_pattern.h"
static hdUiStringTable sMiiverseStringVtable;
static hdUiSafeString sMiiversePatternName = {0, 0};
static u32 sMiiversePatternReady = 0;
static u32 sMiiversePatternRegistration[3] = {0, 0, 0};

/* 026A3FCC */
void hdMenuMiiverse_c::findBackgroundPattern() {
    if (!sMiiversePatternReady) {
        sMiiversePatternReady = 1;
        sMiiversePatternName.mVtable = &sMiiverseStringVtable;
        sMiiversePatternName.mText.v = gabi::guest_addr("P_BgPattern_00");
        miiverseRegisterStringNames(sMiiversePatternRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    mpBackgroundPattern = miiverseFindNamedPane(root, &sMiiversePatternName);
}

/* WWHD glue (tools/verify/autoglue.py): Miiverse background
 data sMiiverseStringVtable 100FBB3C nocheck
 data sMiiversePatternName 1049761C nocheck
 data sMiiversePatternReady 104976D0 nocheck
 data sMiiversePatternRegistration 101F6404 nocheck
 str 100FBDEC "P_BgPattern_00"
*/

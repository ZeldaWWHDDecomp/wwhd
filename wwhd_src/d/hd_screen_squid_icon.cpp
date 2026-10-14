/**
 * hd_screen_squid_icon.cpp
 * HD - the squid icon "SquidIcon_00": a "OneTime" parts layout (one "All" animation, group
 * "G_All_00")
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout and group names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_squid_icon.h"

static hdPartsVtbl_c sSquidIconVtable;
static u32 sSquidIconHolderVtable;
static u32 sSquidIconSecondaryVtable;
static hdUiStringTable sSquidIconSafeStringVtable;
static hdUiSafeString sSquidIconGroupName; /* not used by this code */
static hdUiSafeString sSquidIconLayoutName;

/* 026DFD14 */
// hdScreenSquidIcon_c::~hdScreenSquidIcon_c() (GHS deleting destructor)
void hdScreenSquidIcon_c::destroy(hdScreenSquidIcon_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sSquidIconHolderVtable;
    i_this->mSecondaryVtable = &sSquidIconSecondaryVtable;
    i_this->OneTimePart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026DFD80 */
// hdScreenSquidIcon_c::hdScreenSquidIcon_c() (GHS: allocates when called with NULL)
hdScreenSquidIcon_c* hdScreenSquidIcon_c::construct(hdScreenSquidIcon_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenSquidIcon_c*)operator_new(sizeof(hdScreenSquidIcon_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    OneTimePart_l::construct(i_this);
    i_this->mVtable = &sSquidIconVtable;
    i_this->mStateHolder.mHolderVtable = &sSquidIconHolderVtable;
    i_this->mSecondaryVtable = &sSquidIconSecondaryVtable;
    return i_this;
}

/* 026DFDEC */
u32 hdScreenSquidIcon_c::init(u32 arg1, u32 arg2, void* desc) {
    // the merged base names its arguments (archive, resource, mode, flags); this layout passes the parts
    // descriptor in the last one
    return create(&sSquidIconLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc));
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026DFE04 */
static void __sinit_hd_screen_squid_icon_cpp() {
    sinit_header_statics_rodata(0x1049BB14, 0x101F6B98, 0x1049BB20, 0x10105204);
    hdParts_setSafeString(&sSquidIconGroupName, &sSquidIconSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sSquidIconLayoutName, &sSquidIconSafeStringVtable, "SquidIcon_00");
}

/* WWHD glue
data sSquidIconVtable 10105228 nocheck
data sSquidIconHolderVtable 101052F0 nocheck
data sSquidIconSecondaryVtable 10105300 nocheck
data sSquidIconSafeStringVtable 101051E4 nocheck
data sSquidIconGroupName 1049BB0C nocheck
data sSquidIconLayoutName 1049BB30 nocheck
str 1010520C "G_All_00" in 026DFE04
str 10105218 "SquidIcon_00" in 026DFE04
*/

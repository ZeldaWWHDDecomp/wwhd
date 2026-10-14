/**
 * hd_screen_cannonball.cpp
 * HD - the cannonball icon "Cannonball_00": a "OneTime" parts layout (one "All" animation, group
 * "G_All_00")
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout and group names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_cannonball.h"

static hdPartsVtbl_c sCannonballVtable;
static u32 sCannonballHolderVtable;
static u32 sCannonballSecondaryVtable;
static hdUiStringTable sCannonballSafeStringVtable;
static hdUiSafeString sCannonballGroupName; /* not used by this code */
static hdUiSafeString sCannonballLayoutName;

/* 02645F4C */
// hdScreenCannonball_c::~hdScreenCannonball_c() (GHS deleting destructor)
void hdScreenCannonball_c::destroy(hdScreenCannonball_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sCannonballHolderVtable;
    i_this->mSecondaryVtable = &sCannonballSecondaryVtable;
    i_this->OneTimePart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 02645FB8 */
// hdScreenCannonball_c::hdScreenCannonball_c() (GHS: allocates when called with NULL)
hdScreenCannonball_c* hdScreenCannonball_c::construct(hdScreenCannonball_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenCannonball_c*)operator_new(sizeof(hdScreenCannonball_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    OneTimePart_l::construct(i_this);
    i_this->mVtable = &sCannonballVtable;
    i_this->mStateHolder.mHolderVtable = &sCannonballHolderVtable;
    i_this->mSecondaryVtable = &sCannonballSecondaryVtable;
    return i_this;
}

/* 02646024 */
u32 hdScreenCannonball_c::init(u32 arg1, u32 arg2, void* desc) {
    // the merged base names its arguments (archive, resource, mode, flags); this layout passes the parts
    // descriptor in the last one
    return create(&sCannonballLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc));
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0264603C */
static void __sinit_hd_screen_cannonball_cpp() {
    sinit_header_statics_rodata(0x10490F04, 0x101F57F8, 0x10490F10, 0x100ED154);
    hdParts_setSafeString(&sCannonballGroupName, &sCannonballSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sCannonballLayoutName, &sCannonballSafeStringVtable, "Cannonball_00");
}

/* WWHD glue
data sCannonballVtable 100ED178 nocheck
data sCannonballHolderVtable 100ED240 nocheck
data sCannonballSecondaryVtable 100ED250 nocheck
data sCannonballSafeStringVtable 100ED134 nocheck
data sCannonballGroupName 10490EFC nocheck
data sCannonballLayoutName 10490F20 nocheck
str 100ED16C "G_All_00" in 0264603C
str 100ED15C "Cannonball_00" in 0264603C
ext 02702574 void OneTimePart_l::destroy(u32)
ext 027025E0 OneTimePart_l* OneTimePart_l::construct(OneTimePart_l*)
ext 02702674 u32 OneTimePart_l::create(void*,void*,u32,u32)
*/

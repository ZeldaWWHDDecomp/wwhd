/**
 * hd_screen_battleship_2.cpp
 * HD - the ship icon of the battleship minigame (Battleship_00) "Battleship_00": a "OneTime" parts layout (one "All" animation, group
 * "G_All_00")
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout and group names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_battleship_2.h"

static hdPartsVtbl_c sBattleshipVtable;
static u32 sBattleshipHolderVtable;
static u32 sBattleshipSecondaryVtable;
static hdUiStringTable sBattleshipSafeStringVtable;
static hdUiSafeString sBattleshipGroupName; /* not used by this code */
static hdUiSafeString sBattleshipLayoutName;

/* 0262388C */
// hdScreenBattleshipIcon_c::~hdScreenBattleshipIcon_c() (GHS deleting destructor)
void hdScreenBattleshipIcon_c::destroy(hdScreenBattleshipIcon_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sBattleshipHolderVtable;
    i_this->mSecondaryVtable = &sBattleshipSecondaryVtable;
    i_this->OneTimePart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026238F8 */
// hdScreenBattleshipIcon_c::hdScreenBattleshipIcon_c() (GHS: allocates when called with NULL)
hdScreenBattleshipIcon_c* hdScreenBattleshipIcon_c::construct(hdScreenBattleshipIcon_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenBattleshipIcon_c*)operator_new(sizeof(hdScreenBattleshipIcon_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    OneTimePart_l::construct(i_this);
    i_this->mVtable = &sBattleshipVtable;
    i_this->mStateHolder.mHolderVtable = &sBattleshipHolderVtable;
    i_this->mSecondaryVtable = &sBattleshipSecondaryVtable;
    return i_this;
}

/* 02623964 */
u32 hdScreenBattleshipIcon_c::init(u32 arg1, u32 arg2, void* desc) {
    // the merged base names its arguments (archive, resource, mode, flags); this layout passes the parts
    // descriptor in the last one
    return create(&sBattleshipLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc));
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0262397C */
static void __sinit_hd_screen_battleship_2_cpp() {
    sinit_header_statics_rodata(0x1048EBE0, 0x101F529C, 0x1048EBEC, 0x100E48B4);
    hdParts_setSafeString(&sBattleshipGroupName, &sBattleshipSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sBattleshipLayoutName, &sBattleshipSafeStringVtable, "Battleship_00");
}

/* 0262378C */
static s32 hdScreenBattleshipIcon_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sBattleshipVtable 100E48D8 nocheck
data sBattleshipHolderVtable 100E49A0 nocheck
data sBattleshipSecondaryVtable 100E49B0 nocheck
data sBattleshipSafeStringVtable 100E4894 nocheck
data sBattleshipGroupName 1048EBD8 nocheck
data sBattleshipLayoutName 1048EBFC nocheck
str 100E48CC "G_All_00" in 0262397C
str 100E48BC "Battleship_00" in 0262397C
*/

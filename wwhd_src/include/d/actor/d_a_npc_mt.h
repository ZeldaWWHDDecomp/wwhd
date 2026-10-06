#pragma once
#include "bindings.h"

// WWHD Carlov actor; the profile and allocating constructor both specify 0x8CC.
struct daNpcMt_c : fopAc_ac_c {
    u8 npcStorage[0x8CC-sizeof(fopAc_ac_c)];
};
static_assert(sizeof(daNpcMt_c)==0x8CC);
